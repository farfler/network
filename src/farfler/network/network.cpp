#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <farfler/network/network.hpp>
#include <iostream>

namespace farfler::network {

std::string GenerateId() {
  boost::uuids::random_generator generator;
  boost::uuids::uuid uuid = generator();
  return boost::uuids::to_string(uuid);
}

Network::Network(boost::asio::io_context& io_context, const std::string& name)
    : io_context_(io_context),
      name_(name),
      udp_socket_(io_context),
      tcp_acceptor_(io_context),
      cycle_discovery_messages_timer_(io_context),
      id_(GenerateId()),
      pubsub_(),
      strand_(io_context) {
  if (!instance) {
    instance = std::unique_ptr<Network>(new Network(io_context_, name_, true));
    return;
  }

  InitializeUdpSocket();
  InitializeTcpAcceptor();
  StartReceivingUdpMessages();
  StartAcceptingTcpConnections();
  StartCyclingDiscoveryMessages();
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

void Network::InitializeTcpAcceptor() {
  tcp_acceptor_.open(boost::asio::ip::tcp::v4());
  tcp_acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  tcp_acceptor_.bind(
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0));
  tcp_acceptor_.listen();

  std::cout << "TCP listening on: "
            << tcp_acceptor_.local_endpoint().address().to_string() << ":"
            << tcp_acceptor_.local_endpoint().port() << std::endl
            << std::endl;
}

void Network::StartReceivingUdpMessages() {
  udp_socket_.async_receive_from(
      boost::asio::buffer(recv_buffer_), udp_endpoint_,
      boost::asio::bind_executor(
          strand_,
          [this](const boost::system::error_code& error, std::size_t size) {
            if (error) {
              std::cerr << "Error in UDP receive: " << error.message()
                        << std::endl;
            } else {
              std::vector<char> packet(recv_buffer_.begin(),
                                       recv_buffer_.begin() + size);
              ProcessUdpMessage(packet);
            }
            StartReceivingUdpMessages();
          }));
}

void Network::StartAcceptingTcpConnections() {
  tcp_acceptor_.async_accept(boost::asio::bind_executor(
      strand_, [this](const boost::system::error_code& error,
                      boost::asio::ip::tcp::socket socket) {
        if (!error) {
          std::cout << "Accepted connection from "
                    << socket.remote_endpoint().address().to_string() << ":"
                    << socket.remote_endpoint().port() << std::endl;

          auto shared_socket =
              std::make_shared<boost::asio::ip::tcp::socket>(std::move(socket));
          StartReceivingTcpMessages(shared_socket);
        } else {
          std::cerr << "Error accepting TCP connection: " << error.message()
                    << std::endl;
        }
        StartAcceptingTcpConnections();
      }));
}

void Network::StartCyclingDiscoveryMessages() {
  UdpPing ping;
  ping.id_ = id_;
  ping.name_ = name_;
  ping.udp_address_ = udp_socket_.local_endpoint().address().to_string();
  ping.udp_port_ = udp_socket_.local_endpoint().port();
  ping.tcp_address_ = tcp_acceptor_.local_endpoint().address().to_string();
  ping.tcp_port_ = tcp_acceptor_.local_endpoint().port();
  UdpBroadcast(UdpPing::Serialize(ping));

  cycle_discovery_messages_timer_.expires_after(std::chrono::seconds(1));
  cycle_discovery_messages_timer_.async_wait(boost::asio::bind_executor(
      strand_, [this](const boost::system::error_code& error) {
        if (!error) {
          StartCyclingDiscoveryMessages();
        } else {
          std::cerr << "Error in discovery cycle: " << error.message()
                    << std::endl;
        }
      }));
}

void Network::UdpBroadcast(const std::vector<char>& packet) {
  boost::asio::ip::address_v4 address =
      boost::asio::ip::address_v4::broadcast();
  boost::asio::ip::udp::endpoint endpoint =
      boost::asio::ip::udp::endpoint(address, 21075);
  udp_socket_.async_send_to(
      boost::asio::buffer(packet), endpoint,
      boost::asio::bind_executor(
          strand_,
          [this](const boost::system::error_code& error, std::size_t size) {
            if (error) {
              std::cerr << "Error in UDP broadcast: " << error.message()
                        << std::endl;
            }
          }));
}

void Network::ProcessUdpMessage(const std::vector<char>& packet) {
  std::vector<char> mutable_packet(packet.begin(), packet.end());
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
    pong_msg.tcp_address_ =
        tcp_acceptor_.local_endpoint().address().to_string();
    pong_msg.tcp_port_ = tcp_acceptor_.local_endpoint().port();
    UdpBroadcast(UdpPong::Serialize(pong_msg));
  }
}

void Network::HandleUdpPong(std::vector<char>& packet) {
  UdpPong msg = UdpPong::Deserialize(packet);

  if (msg.id_ != id_) {
    std::lock_guard<std::mutex> lock(tcp_sockets_mutex_);
    if (unverified_tcp_sockets_.find(msg.id_) ==
            unverified_tcp_sockets_.end() &&
        connecting_tcp_sockets_.find(msg.id_) ==
            connecting_tcp_sockets_.end()) {
      ConnectToPeer(msg);
    }
  }
}

void Network::ConnectToPeer(const UdpPong& msg) {
  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(
      tcp_acceptor_.get_executor());
  boost::asio::ip::tcp::resolver resolver(tcp_acceptor_.get_executor());
  auto endpoints =
      resolver.resolve(msg.tcp_address_, std::to_string(msg.tcp_port_));

  boost::asio::async_connect(
      *socket, endpoints,
      boost::asio::bind_executor(
          strand_,
          [this, socket, msg](const boost::system::error_code& error,
                              const boost::asio::ip::tcp::endpoint& endpoint) {
            if (!error) {
              std::cout << "Connected to " << endpoint.address().to_string()
                        << ":" << endpoint.port() << std::endl;
              {
                std::lock_guard<std::mutex> lock(tcp_sockets_mutex_);
                unverified_tcp_sockets_[msg.id_] = socket;
              }
              StartReceivingTcpMessages(socket);
              SendTcpPing(socket);
            } else {
              std::cerr << "Error connecting to peer: " << error.message()
                        << std::endl;
            }
          }));
}

void Network::StartReceivingTcpMessages(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  auto header_buffer = std::make_shared<std::array<char, 4>>();
  boost::asio::async_read(
      *socket, boost::asio::buffer(*header_buffer),
      boost::asio::bind_executor(
          strand_,
          [this, socket, header_buffer](const boost::system::error_code& error,
                                        std::size_t size) {
            if (error) {
              HandleTcpError(socket, error);
              return;
            }

            std::vector<char> header_vec(header_buffer->begin(),
                                         header_buffer->begin() + size);
            uint32_t packet_size = UInt32::Deserialize(header_vec);

            auto message_buffer =
                std::make_shared<std::vector<char>>(packet_size);
            boost::asio::async_read(
                *socket, boost::asio::buffer(*message_buffer),
                boost::asio::bind_executor(
                    strand_, [this, socket, message_buffer](
                                 const boost::system::error_code& error,
                                 std::size_t size) {
                      if (error) {
                        HandleTcpError(socket, error);
                        return;
                      }

                      ProcessTcpMessage(*message_buffer, socket);
                      StartReceivingTcpMessages(socket);
                    }));
          }));
}

void Network::HandleTcpError(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket,
    const boost::system::error_code& error) {
  std::cerr << "TCP error: " << error.message() << std::endl;
  std::lock_guard<std::mutex> lock(tcp_sockets_mutex_);
  for (auto it = connecting_tcp_sockets_.begin();
       it != connecting_tcp_sockets_.end();) {
    if (it->second == socket) {
      std::cout << "Removing disconnected peer: " << it->first << std::endl;
      it = connecting_tcp_sockets_.erase(it);
    } else {
      ++it;
    }
  }
  for (auto it = unverified_tcp_sockets_.begin();
       it != unverified_tcp_sockets_.end();) {
    if (it->second == socket) {
      std::cout << "Removing unverified peer: " << it->first << std::endl;
      it = unverified_tcp_sockets_.erase(it);
    } else {
      ++it;
    }
  }
}

void Network::ProcessTcpMessage(
    const std::vector<char>& packet,
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  std::vector<char> mutable_packet(packet.begin(), packet.end());
  std::string message_type = String::Deserialize(mutable_packet);

  if (message_type == "tcp_ping") {
    HandleTcpPing(mutable_packet, socket);
  } else if (message_type == "tcp_pong") {
    HandleTcpPong(mutable_packet, socket);
  } else if (message_type == "publication") {
    HandlePublication(mutable_packet);
  }
}

void Network::HandleTcpPing(
    std::vector<char>& packet,
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  TcpPing msg = TcpPing::Deserialize(packet);

  std::cout << "Got tcp_ping from " << msg.id_ << " " << msg.name_ << std::endl;

  TcpPong pong_msg;
  pong_msg.id_ = id_;
  pong_msg.name_ = name_;
  pong_msg.udp_address_ = udp_socket_.local_endpoint().address().to_string();
  pong_msg.udp_port_ = udp_socket_.local_endpoint().port();
  pong_msg.tcp_address_ = tcp_acceptor_.local_endpoint().address().to_string();
  pong_msg.tcp_port_ = tcp_acceptor_.local_endpoint().port();
  pong_msg.subscribed_topics_ = pubsub_.GetOnlineSubscribedTopics();

  SendTcpMessage(socket, TcpPong::Serialize(pong_msg));
}

void Network::HandleTcpPong(
    std::vector<char>& packet,
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  TcpPong msg = TcpPong::Deserialize(packet);

  std::cout << "Received tcp_pong from " << msg.id_ << " " << msg.name_
            << std::endl;

  UpdatePeerSubscriptions(msg.id_, msg.subscribed_topics_);

  {
    std::lock_guard<std::mutex> lock(tcp_sockets_mutex_);
    connecting_tcp_sockets_[msg.id_] = socket;
    unverified_tcp_sockets_.erase(msg.id_);
  }

  std::cout << "Verified tcp sockets: " << connecting_tcp_sockets_.size()
            << std::endl;
}

void Network::HandlePublication(std::vector<char>& packet) {
  std::string topic = String::Deserialize(packet);
  uint32_t message_size;
  UInt32::Deserialize(packet, message_size);
  std::vector<char> message(packet.begin(), packet.begin() + message_size);
  pubsub_.PublishOnline(topic, message);
}

void Network::UpdatePeerSubscriptions(const std::string& peer_id,
                                      const std::vector<std::string>& topics) {
  std::cout << "Updating subscriptions for peer " << peer_id << ":"
            << std::endl;
  for (const auto& topic : topics) {
    std::cout << "  " << topic << std::endl;
  }
}

void Network::SendTcpPing(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
  TcpPing ping;
  ping.id_ = id_;
  ping.name_ = name_;
  ping.udp_address_ = udp_socket_.local_endpoint().address().to_string();
  ping.udp_port_ = udp_socket_.local_endpoint().port();
  ping.tcp_address_ = tcp_acceptor_.local_endpoint().address().to_string();
  ping.tcp_port_ = tcp_acceptor_.local_endpoint().port();
  ping.subscribed_topics_ = pubsub_.GetOnlineSubscribedTopics();
  std::vector<char> packet = TcpPing::Serialize(ping);

  SendTcpMessage(socket, packet);
}

void Network::SendTcpMessage(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket,
    const std::vector<char>& message) {
  uint32_t size = message.size();
  std::vector<char> buffer;
  UInt32::Serialize(size, buffer);
  buffer.insert(buffer.end(), message.begin(), message.end());

  boost::asio::async_write(
      *socket, boost::asio::buffer(buffer),
      boost::asio::bind_executor(
          strand_, [this, socket](const boost::system::error_code& error,
                                  std::size_t size) {
            if (error) {
              std::cerr << "Error sending TCP message: " << error.message()
                        << std::endl;
              HandleTcpError(socket, error);
            }
          }));
}

void Network::SendPublication(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket,
    const std::string& topic, const std::vector<char>& message) {
  std::vector<char> packet;
  String::Serialize("publication", packet);
  String::Serialize(topic, packet);
  UInt32::Serialize(message.size(), packet);
  packet.insert(packet.end(), message.begin(), message.end());

  SendTcpMessage(socket, packet);
}

void Network::BroadcastSubscriptionUpdate() {
  TcpPing ping;
  ping.id_ = id_;
  ping.name_ = name_;
  ping.udp_address_ = udp_socket_.local_endpoint().address().to_string();
  ping.udp_port_ = udp_socket_.local_endpoint().port();
  ping.tcp_address_ = tcp_acceptor_.local_endpoint().address().to_string();
  ping.tcp_port_ = tcp_acceptor_.local_endpoint().port();
  ping.subscribed_topics_ = pubsub_.GetOnlineSubscribedTopics();

  std::vector<char> packet = TcpPing::Serialize(ping);

  std::lock_guard<std::mutex> lock(tcp_sockets_mutex_);
  for (const auto& [id, socket] : connecting_tcp_sockets_) {
    SendTcpMessage(socket, packet);
  }
}

void Network::UnsubscribeOffline(const std::string& topic,
                                 const Subscription& subscription) {
  if (!instance) {
    std::cout << "Initialize a network instance first" << std::endl;
    return;
  }

  UnsubscribeOffline(*instance, topic, subscription);
}

void Network::UnsubscribeOffline(Network& network, const std::string& topic,
                                 const Subscription& subscription) {
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  network.pubsub_.UnsubscribeOffline(topic, subscription);
}

void Network::UnsubscribeOnline(const std::string& topic,
                                const Subscription& subscription) {
  if (!instance) {
    std::cout << "Initialize a network instance first" << std::endl;
    return;
  }

  UnsubscribeOnline(*instance, topic, subscription);
}

void Network::UnsubscribeOnline(Network& network, const std::string& topic,
                                const Subscription& subscription) {
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  network.pubsub_.UnsubscribeOnline(topic, subscription);
  network.BroadcastSubscriptionUpdate();
}

void Network::UnsubscribeAll(const std::string& topic,
                             const Subscription& subscription) {
  if (!instance) {
    std::cout << "Initialize a network instance first" << std::endl;
    return;
  }

  UnsubscribeAll(*instance, topic, subscription);
}

void Network::UnsubscribeAll(Network& network, const std::string& topic,
                             const Subscription& subscription) {
  std::lock_guard<std::mutex> lock(network.pubsub_mutex_);
  network.pubsub_.UnsubscribeAll(topic, subscription);
  network.BroadcastSubscriptionUpdate();
}

Network::Network(boost::asio::io_context& io_context, const std::string& name,
                 bool)
    : io_context_(io_context),
      name_(name),
      udp_socket_(io_context),
      tcp_acceptor_(io_context),
      cycle_discovery_messages_timer_(io_context),
      id_(GenerateId()),
      pubsub_(),
      strand_(io_context) {
  std::cout << "Initialized down here" << std::endl;
  InitializeUdpSocket();
  InitializeTcpAcceptor();
  StartReceivingUdpMessages();
  StartAcceptingTcpConnections();
  StartCyclingDiscoveryMessages();
}

std::unique_ptr<Network> Network::instance = nullptr;

}  // namespace farfler::network