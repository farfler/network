#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <farfler/network/pingpong.hpp>
#include <farfler/network/pubsub.hpp>
#include <farfler/network/types.hpp>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace farfler::network {

class Network {
 public:
  Network(boost::asio::io_context& io_context, const std::string& name);

  template <typename T>
  static void PublishOffline(Network& network, const std::string& topic,
                             const T& message);

  template <typename T>
  static void PublishOnline(Network& network, const std::string& topic,
                            const T& message);

  template <typename T>
  static void PublishAll(Network& network, const std::string& topic,
                         const T& message);

  template <typename Callback>
  static Subscription SubscribeOffline(Network& network,
                                       const std::string& topic,
                                       Callback callback);

  template <typename Callback>
  static Subscription SubscribeOnline(Network& network,
                                      const std::string& topic,
                                      Callback callback);

  template <typename Callback>
  static Subscription SubscribeAll(Network& network, const std::string& topic,
                                   Callback callback);

  static void UnsubscribeOffline(Network& network, const std::string& topic,
                                 const Subscription& subscription);
  static void UnsubscribeOnline(Network& network, const std::string& topic,
                                const Subscription& subscription);
  static void UnsubscribeAll(Network& network, const std::string& topic,
                             const Subscription& subscription);

 private:
  void InitializeUdpSocket();
  void InitializeTcpAcceptor();
  void StartReceivingUdpMessages();
  void StartAcceptingTcpConnections();
  void StartCyclingDiscoveryMessages();
  void UdpBroadcast(const std::vector<char>& packet);
  void ProcessUdpMessage(const std::vector<char>& packet);
  void HandleUdpPing(std::vector<char>& packet);
  void HandleUdpPong(std::vector<char>& packet);
  void ConnectToPeer(const UdpPong& msg);
  void StartReceivingTcpMessages(
      std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void HandleTcpError(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                      const boost::system::error_code& error);
  void ProcessTcpMessage(const std::vector<char>& packet,
                         std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void HandleTcpPing(std::vector<char>& packet,
                     std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void HandleTcpPong(std::vector<char>& packet,
                     std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void HandlePublication(std::vector<char>& packet);
  void UpdatePeerSubscriptions(const std::string& peer_id,
                               const std::vector<std::string>& topics);
  void SendTcpPing(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void SendTcpMessage(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                      const std::vector<char>& message);
  void SendPublication(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                       const std::string& topic,
                       const std::vector<char>& message);
  void BroadcastSubscriptionUpdate();

  template <typename Callback, typename T>
  static Subscription SubscribeOfflineImpl(Network& network,
                                           const std::string& topic,
                                           Callback callback,
                                           void (Callback::*)(const T&) const);

  template <typename Callback, typename T>
  static Subscription SubscribeOnlineImpl(Network& network,
                                          const std::string& topic,
                                          Callback callback,
                                          void (Callback::*)(const T&) const);

  template <typename Callback, typename T>
  static Subscription SubscribeAllImpl(Network& network,
                                       const std::string& topic,
                                       Callback callback,
                                       void (Callback::*)(const T&) const);

  boost::asio::io_context& io_context_;
  boost::asio::ip::udp::socket udp_socket_;
  boost::asio::ip::tcp::acceptor tcp_acceptor_;
  boost::asio::ip::udp::endpoint udp_endpoint_;
  boost::asio::steady_timer cycle_discovery_messages_timer_;
  std::unordered_map<std::string, std::shared_ptr<boost::asio::ip::tcp::socket>>
      connecting_tcp_sockets_;
  std::unordered_map<std::string, std::shared_ptr<boost::asio::ip::tcp::socket>>
      unverified_tcp_sockets_;
  std::array<char, 1024> recv_buffer_;
  std::string id_;
  std::string name_;
  PubSub pubsub_;
  std::mutex tcp_sockets_mutex_;
  std::mutex pubsub_mutex_;
  boost::asio::io_context::strand strand_;
};

}  // namespace farfler::network

#include "network.tpp"