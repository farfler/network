#include <farfler/network/pubsub.hpp>

namespace farfler::network {

Subscription::Subscription(const std::string& id) : id_(id) {}

void PubSub::UnsubscribeOffline(const std::string& topic,
                                const Subscription& subscription) {
  std::lock_guard<std::mutex> lock(mutex_);
  offline_subscribers_[topic].erase(subscription.id_);
}

void PubSub::UnsubscribeOnline(const std::string& topic,
                               const Subscription& subscription) {
  std::lock_guard<std::mutex> lock(mutex_);
  online_subscribers_[topic].erase(subscription.id_);
}

void PubSub::PublishOffline(const std::string& topic,
                            const std::vector<char>& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  for (const auto& [id, subscriber] : offline_subscribers_[topic]) {
    subscriber(message);
  }
}

void PubSub::PublishOnline(const std::string& topic,
                           const std::vector<char>& message) {
  std::lock_guard<std::mutex> lock(mutex_);
  for (const auto& [id, subscriber] : online_subscribers_[topic]) {
    subscriber(message);
  }
}

void PubSub::Unsubscribe(const std::string& topic,
                            const Subscription& subscription) {
  std::lock_guard<std::mutex> lock(mutex_);
  offline_subscribers_[topic].erase(subscription.id_);
  online_subscribers_[topic].erase(subscription.id_);
}

std::vector<std::string> PubSub::GetOnlineSubscribedTopics() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<std::string> topics;
  for (const auto& [topic, subscribers] : online_subscribers_) {
    if (!subscribers.empty()) {
      topics.push_back(topic);
    }
  }
  return topics;
}

std::string PubSub::GenerateSubscriptionId() {
  boost::uuids::random_generator generator;
  boost::uuids::uuid uuid = generator();
  return boost::uuids::to_string(uuid);
}

}  // namespace farfler::network