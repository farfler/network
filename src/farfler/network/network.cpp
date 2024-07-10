#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <farfler/network/network.hpp>
#include <farfler/network/pingpong.hpp>
#include <iostream>

namespace farfler::network {

Network::Network(boost::asio::io_context& io_context, const std::string& name)
    : io_context_(io_context),
      udp_socket_(io_context),
      name_(name),
      id_(boost::uuids::to_string(boost::uuids::random_generator()())) {
  InitializeUdpSocket();
  StartReceivingUdpMessages();
}

void Network::InitializeUdpSocket() {
  udp_socket_.open(boost::asio::ip::udp::v4());
  udp_socket_.set_option(boost::asio::socket_base::broadcast(true));
  udp_socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
  udp_socket_.bind(
      boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 21075));

  std::cout << "UDP listening on: "
            << udp_socket_.local_endpoint().address().to_string() << ":"
            << udp_socket_.local_endpoint().port() << std::endl;
}

void Network::StartReceivingUdpMessages() {
  udp_socket_.async_receive_from(
      boost::asio::buffer(udp_receive_buffer_), udp_remote_endpoint_,
      std::bind(&Network::HandleUdpReceive, this, std::placeholders::_1,
                std::placeholders::_2));
}

void Network::HandleUdpReceive(const boost::system::error_code& error,
                               std::size_t bytes_transferred) {
  if (!error) {
    std::vector<char> packet(udp_receive_buffer_.begin(),
                             udp_receive_buffer_.begin() + bytes_transferred);
    ProcessUdpMessage(packet);
  } else {
    std::cerr << "Error in UDP receive: " << error.message() << std::endl;
  }

  StartReceivingUdpMessages();
}

void Network::ProcessUdpMessage(const std::vector<char>& packet) {
  std::vector<char> mutable_packet = packet;
  std::string type = String::Deserialize(mutable_packet);

  if (type == "udp_ping") {
    HandleUdpPing(mutable_packet);
  } else if (type == "udp_pong") {
    HandleUdpPong(mutable_packet);
  }
}

void Network::HandleUdpPing(std::vector<char>& packet) {
  UdpPing msg = UdpPing::Deserialize(packet);

  if (msg.id_ != id_) {
    UdpPong pong_msg;
    pong_msg.id_ = id_;
    pong_msg.name_ = name_;
    pong_msg.udp_address_ = udp_socket_.local_endpoint().address().to_string();
    pong_msg.udp_port_ = udp_socket_.local_endpoint().port();

    SendUdpBroadcast(UdpPong::Serialize(pong_msg));
  }
}

void Network::HandleUdpPong(std::vector<char>& packet) {
  UdpPong msg = UdpPong::Deserialize(packet);

  if (msg.id_ != id_) {
    std::cout << "Received UDP pong from " << msg.id_ << " (" << msg.name_
              << ")" << std::endl;
  }
}

void Network::SendUdpBroadcast(const std::vector<char>& packet) {
  boost::asio::ip::udp::endpoint broadcast_endpoint(
      boost::asio::ip::address_v4::broadcast(), 21075);

  udp_socket_.async_send_to(boost::asio::buffer(packet), broadcast_endpoint,
                            [](const boost::system::error_code& error,
                               std::size_t bytes_transferred) {
                              if (error) {
                                std::cerr << "Error in UDP broadcast: "
                                          << error.message() << std::endl;
                              }
                            });
}

}  // namespace farfler::network