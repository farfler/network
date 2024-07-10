#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace farfler::network {

class Subscription {
 public:
  explicit Subscription(const std::string& id = "");
  std::string id_;
};

class PubSub {
 public:
  template <typename Callback>
  Subscription SubscribeOffline(const std::string& topic, Callback callback);

  template <typename Callback>
  Subscription SubscribeOnline(const std::string& topic, Callback callback);

  void UnsubscribeOffline(const std::string& topic,
                          const Subscription& subscription);
  void UnsubscribeOnline(const std::string& topic,
                         const Subscription& subscription);

  void PublishOffline(const std::string& topic,
                      const std::vector<char>& message);
  void PublishOnline(const std::string& topic,
                     const std::vector<char>& message);

  template <typename Callback>
  Subscription SubscribeAll(const std::string& topic, Callback callback);

  void UnsubscribeAll(const std::string& topic,
                      const Subscription& subscription);

  std::vector<std::string> GetOnlineSubscribedTopics() const;

 private:
  std::string GenerateSubscriptionId();

  std::unordered_map<
      std::string,
      std::unordered_map<std::string,
                         std::function<void(const std::vector<char>&)>>>
      offline_subscribers_;
  std::unordered_map<
      std::string,
      std::unordered_map<std::string,
                         std::function<void(const std::vector<char>&)>>>
      online_subscribers_;
  mutable std::mutex mutex_;
};

}  // namespace farfler::network

#include "pubsub.tpp"
