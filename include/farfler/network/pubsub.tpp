namespace farfler::network {

template <typename Callback>
Subscription PubSub::SubscribeOffline(const std::string &topic,
                                      Callback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::string id = GenerateSubscriptionId();
  offline_subscribers_[topic][id] = callback;
  return Subscription(id);
}

template <typename Callback>
Subscription PubSub::SubscribeOnline(const std::string &topic,
                                     Callback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::string id = GenerateSubscriptionId();
  online_subscribers_[topic][id] = callback;
  return Subscription(id);
}

template <typename Callback>
Subscription PubSub::SubscribeAll(const std::string &topic, Callback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::string id = GenerateSubscriptionId();
  offline_subscribers_[topic][id] = callback;
  online_subscribers_[topic][id] = callback;
  return Subscription(id);
}

}  // namespace farfler::network