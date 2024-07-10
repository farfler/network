#pragma once

#include <boost/asio.hpp>
#include <farfler/network/pubsub.hpp>
#include <farfler/network/types.hpp>
#include <memory>
#include <string>
#include <vector>

namespace farfler::network {

class Network {
 public:
  Network(boost::asio::io_context& io_context, const std::string& name);

  void StartReceivingUdpMessages();
  void SendUdpBroadcast(const std::vector<char>& packet);

 private:
  void InitializeUdpSocket();
  void HandleUdpReceive(const boost::system::error_code& error,
                        std::size_t bytes_transferred);
  void ProcessUdpMessage(const std::vector<char>& packet);
  void HandleUdpPing(std::vector<char>& packet);
  void HandleUdpPong(std::vector<char>& packet);

  boost::asio::io_context& io_context_;
  boost::asio::ip::udp::socket udp_socket_;
  boost::asio::ip::udp::endpoint udp_remote_endpoint_;
  std::array<char, 1024> udp_receive_buffer_;
  std::string id_;
  std::string name_;
};

}  // namespace farfler::network