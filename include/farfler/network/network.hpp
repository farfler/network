#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <farfler/network/pingpong.hpp>
#include <farfler/network/types.hpp>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace farfler::network {

class Network {
 public:
  Network(boost::asio::io_context& io_context, const std::string& name);

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
  void SendTcpPing(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  void SendTcpMessage(std::shared_ptr<boost::asio::ip::tcp::socket> socket,
                      const std::vector<char>& message);

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
  std::mutex tcp_sockets_mutex_;
  boost::asio::io_context::strand strand_;
};

}  // namespace farfler::network