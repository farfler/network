namespace farfler::network {

template <typename T>
void Network::PublishOffline(Network& network, const std::string& topic,
                             const T& message) {
  std::vector<char> serialized = T::Serialize(message);
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  network.pubsub_.PublishOffline(topic, serialized);
}

template <typename T>
void Network::PublishOnline(Network& network, const std::string& topic,
                            const T& message) {
  std::vector<char> serialized = T::Serialize(message);
  std::lock_guard<std::mutex> lock(network.tcp_sockets_mutex_);
  for (const auto& [id, socket] : network.connecting_tcp_sockets_) {
    network.SendPublication(socket, topic, serialized);
  }
}

template <typename T>
void Network::PublishAll(Network& network, const std::string& topic,
                         const T& message) {
  PublishOffline<T>(network, topic, message);
  PublishOnline<T>(network, topic, message);
}

template <typename Callback, typename T>
Subscription Network::SubscribeOfflineImpl(Network& network,
                                           const std::string& topic,
                                           Callback callback,
                                           void (Callback::*)(const T&) const) {
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  return network.pubsub_.SubscribeOffline(
      topic, [callback](const std::vector<char>& serialized) {
        std::vector<char> mutable_serialized = serialized;
        T deserialized = T::Deserialize(mutable_serialized);
        callback(deserialized);
      });
}

template <typename Callback, typename T>
Subscription Network::SubscribeOnlineImpl(Network& network,
                                          const std::string& topic,
                                          Callback callback,
                                          void (Callback::*)(const T&) const) {
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  Subscription subscription = network.pubsub_.SubscribeOnline(
      topic, [callback](const std::vector<char>& serialized) {
        std::vector<char> mutable_serialized = serialized;
        T deserialized = T::Deserialize(mutable_serialized);
        callback(deserialized);
      });
  network.BroadcastSubscriptionUpdate();
  return subscription;
}

template <typename Callback, typename T>
Subscription Network::SubscribeAllImpl(Network& network,
                                       const std::string& topic,
                                       Callback callback,
                                       void (Callback::*)(const T&) const) {
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  Subscription subscription = network.pubsub_.SubscribeAll(
      topic, [callback](const std::vector<char>& serialized) {
        std::vector<char> mutable_serialized = serialized;
        T deserialized = T::Deserialize(mutable_serialized);
        callback(deserialized);
      });
  network.BroadcastSubscriptionUpdate();
  return subscription;
}

template <typename Callback>
Subscription Network::SubscribeOffline(Network& network,
                                       const std::string& topic,
                                       Callback callback) {
  return SubscribeOfflineImpl(network, topic, callback, &Callback::operator());
}

template <typename Callback>
Subscription Network::SubscribeOnline(Network& network,
                                      const std::string& topic,
                                      Callback callback) {
  return SubscribeOnlineImpl(network, topic, callback, &Callback::operator());
}

template <typename Callback>
Subscription Network::SubscribeAll(Network& network, const std::string& topic,
                                   Callback callback) {
  return SubscribeAllImpl(network, topic, callback, &Callback::operator());
}

}  // namespace farfler::network