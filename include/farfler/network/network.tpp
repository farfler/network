namespace farfler::network {

template <typename T>
void Network::PublishOffline(const std::string& topic, const T& message) {
  std::vector<char> serialized = T::Serialize(message);
  std::lock_guard<std::mutex> lock(pubsub_mutex_);
  pubsub_.PublishOffline(topic, serialized);
}

template <typename T>
void Network::PublishOnline(const std::string& topic, const T& message) {
  std::vector<char> serialized = T::Serialize(message);
  std::lock_guard<std::mutex> lock(tcp_sockets_mutex_);
  for (const auto& [id, socket] : connecting_tcp_sockets_) {
    SendPublication(socket, topic, serialized);
  }
}

template <typename T>
void Network::PublishAll(const std::string& topic, const T& message) {
  PublishOffline<T>(topic, message);
  PublishOnline<T>(topic, message);
}

template <typename Callback, typename T>
Subscription Network::SubscribeOfflineImpl(const std::string& topic,
                                           Callback callback,
                                           void (Callback::*)(const T&) const) {
  std::lock_guard<std::mutex> lock(pubsub_mutex_);
  return pubsub_.SubscribeOffline(
      topic, [callback](const std::vector<char>& serialized) {
        std::vector<char> mutable_serialized = serialized;
        T deserialized = T::Deserialize(mutable_serialized);
        callback(deserialized);
      });
}

template <typename Callback, typename T>
Subscription Network::SubscribeOnlineImpl(const std::string& topic,
                                          Callback callback,
                                          void (Callback::*)(const T&) const) {
  std::lock_guard<std::mutex> lock(pubsub_mutex_);
  Subscription subscription = pubsub_.SubscribeOnline(
      topic, [callback](const std::vector<char>& serialized) {
        std::vector<char> mutable_serialized = serialized;
        T deserialized = T::Deserialize(mutable_serialized);
        callback(deserialized);
      });
  BroadcastSubscriptionUpdate();
  return subscription;
}

template <typename Callback, typename T>
Subscription Network::SubscribeAllImpl(const std::string& topic,
                                       Callback callback,
                                       void (Callback::*)(const T&) const) {
  std::lock_guard<std::mutex> lock(pubsub_mutex_);
  Subscription subscription = pubsub_.SubscribeAll(
      topic, [callback](const std::vector<char>& serialized) {
        std::vector<char> mutable_serialized = serialized;
        T deserialized = T::Deserialize(mutable_serialized);
        callback(deserialized);
      });
  BroadcastSubscriptionUpdate();
  return subscription;
}

template <typename Callback>
Subscription Network::SubscribeOffline(const std::string& topic,
                                       Callback callback) {
  return SubscribeOfflineImpl(topic, callback, &Callback::operator());
}

template <typename Callback>
Subscription Network::SubscribeOnline(const std::string& topic,
                                      Callback callback) {
  return SubscribeOnlineImpl(topic, callback, &Callback::operator());
}

template <typename Callback>
Subscription Network::SubscribeAll(const std::string& topic,
                                   Callback callback) {
  return SubscribeAllImpl(topic, callback, &Callback::operator());
}

}  // namespace farfler::network