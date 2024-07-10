#include <farfler/network/pingpong.hpp>

namespace farfler::network {

UdpPing::UdpPing(const std::string& id, const std::string& name,
                 const std::string& udp_address, uint16_t udp_port,
                 const std::string& tcp_address, uint16_t tcp_port)
    : id_(id),
      name_(name),
      udp_address_(udp_address),
      udp_port_(udp_port),
      tcp_address_(tcp_address),
      tcp_port_(tcp_port) {}

std::vector<char> UdpPing::Serialize(const UdpPing& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> UdpPing::Serialize(const UdpPing& msg,
                                     std::vector<char>& packet) {
  String::Serialize("udp_ping", packet);
  String::Serialize(msg.id_, packet);
  String::Serialize(msg.name_, packet);
  String::Serialize(msg.udp_address_, packet);
  UInt16::Serialize(msg.udp_port_, packet);
  String::Serialize(msg.tcp_address_, packet);
  UInt16::Serialize(msg.tcp_port_, packet);
  return packet;
}

UdpPing UdpPing::Deserialize(std::vector<char>& packet) {
  UdpPing msg;
  Deserialize(packet, msg);
  return msg;
}

UdpPing UdpPing::Deserialize(std::vector<char>& packet, UdpPing& msg) {
  String::Deserialize(packet, msg.id_);
  String::Deserialize(packet, msg.name_);
  String::Deserialize(packet, msg.udp_address_);
  UInt16::Deserialize(packet, msg.udp_port_);
  String::Deserialize(packet, msg.tcp_address_);
  UInt16::Deserialize(packet, msg.tcp_port_);
  return msg;
}

UdpPong::UdpPong(const std::string& id, const std::string& name,
                 const std::string& udp_address, uint16_t udp_port,
                 const std::string& tcp_address, uint16_t tcp_port)
    : id_(id),
      name_(name),
      udp_address_(udp_address),
      udp_port_(udp_port),
      tcp_address_(tcp_address),
      tcp_port_(tcp_port) {}

std::vector<char> UdpPong::Serialize(const UdpPong& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> UdpPong::Serialize(const UdpPong& msg,
                                     std::vector<char>& packet) {
  String::Serialize("udp_pong", packet);
  String::Serialize(msg.id_, packet);
  String::Serialize(msg.name_, packet);
  String::Serialize(msg.udp_address_, packet);
  UInt16::Serialize(msg.udp_port_, packet);
  String::Serialize(msg.tcp_address_, packet);
  UInt16::Serialize(msg.tcp_port_, packet);
  return packet;
}

UdpPong UdpPong::Deserialize(std::vector<char>& packet) {
  UdpPong msg;
  Deserialize(packet, msg);
  return msg;
}

UdpPong UdpPong::Deserialize(std::vector<char>& packet, UdpPong& msg) {
  String::Deserialize(packet, msg.id_);
  String::Deserialize(packet, msg.name_);
  String::Deserialize(packet, msg.udp_address_);
  UInt16::Deserialize(packet, msg.udp_port_);
  String::Deserialize(packet, msg.tcp_address_);
  UInt16::Deserialize(packet, msg.tcp_port_);
  return msg;
}

TcpPing::TcpPing(const std::string& id, const std::string& name,
                 const std::string& udp_address, uint16_t udp_port,
                 const std::string& tcp_address, uint16_t tcp_port)
    : id_(id),
      name_(name),
      udp_address_(udp_address),
      udp_port_(udp_port),
      tcp_address_(tcp_address),
      tcp_port_(tcp_port) {}

std::vector<char> TcpPing::Serialize(const TcpPing& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> TcpPing::Serialize(const TcpPing& msg,
                                     std::vector<char>& packet) {
  String::Serialize("tcp_ping", packet);
  String::Serialize(msg.id_, packet);
  String::Serialize(msg.name_, packet);
  String::Serialize(msg.udp_address_, packet);
  UInt16::Serialize(msg.udp_port_, packet);
  String::Serialize(msg.tcp_address_, packet);
  UInt16::Serialize(msg.tcp_port_, packet);
  UInt32::Serialize(msg.subscribed_topics_.size(), packet);
  for (const auto& topic : msg.subscribed_topics_) {
    String::Serialize(topic, packet);
  }
  return packet;
}

TcpPing TcpPing::Deserialize(std::vector<char>& packet) {
  TcpPing msg;
  Deserialize(packet, msg);
  return msg;
}

TcpPing TcpPing::Deserialize(std::vector<char>& packet, TcpPing& msg) {
  String::Deserialize(packet, msg.id_);
  String::Deserialize(packet, msg.name_);
  String::Deserialize(packet, msg.udp_address_);
  UInt16::Deserialize(packet, msg.udp_port_);
  String::Deserialize(packet, msg.tcp_address_);
  UInt16::Deserialize(packet, msg.tcp_port_);
  uint32_t topic_count;
  UInt32::Deserialize(packet, topic_count);
  msg.subscribed_topics_.clear();
  for (uint32_t i = 0; i < topic_count; ++i) {
    std::string topic;
    String::Deserialize(packet, topic);
    msg.subscribed_topics_.push_back(topic);
  }
  return msg;
}

TcpPong::TcpPong(const std::string& id, const std::string& name,
                 const std::string& udp_address, uint16_t udp_port,
                 const std::string& tcp_address, uint16_t tcp_port)
    : id_(id),
      name_(name),
      udp_address_(udp_address),
      udp_port_(udp_port),
      tcp_address_(tcp_address),
      tcp_port_(tcp_port) {}

std::vector<char> TcpPong::Serialize(const TcpPong& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> TcpPong::Serialize(const TcpPong& msg,
                                     std::vector<char>& packet) {
  String::Serialize("tcp_pong", packet);
  String::Serialize(msg.id_, packet);
  String::Serialize(msg.name_, packet);
  String::Serialize(msg.udp_address_, packet);
  UInt16::Serialize(msg.udp_port_, packet);
  String::Serialize(msg.tcp_address_, packet);
  UInt16::Serialize(msg.tcp_port_, packet);
  UInt32::Serialize(msg.subscribed_topics_.size(), packet);
  for (const auto& topic : msg.subscribed_topics_) {
    String::Serialize(topic, packet);
  }
  return packet;
}

TcpPong TcpPong::Deserialize(std::vector<char>& packet) {
  TcpPong msg;
  Deserialize(packet, msg);
  return msg;
}

TcpPong TcpPong::Deserialize(std::vector<char>& packet, TcpPong& msg) {
  String::Deserialize(packet, msg.id_);
  String::Deserialize(packet, msg.name_);
  String::Deserialize(packet, msg.udp_address_);
  UInt16::Deserialize(packet, msg.udp_port_);
  String::Deserialize(packet, msg.tcp_address_);
  UInt16::Deserialize(packet, msg.tcp_port_);
  uint32_t topic_count;
  UInt32::Deserialize(packet, topic_count);
  msg.subscribed_topics_.clear();
  for (uint32_t i = 0; i < topic_count; ++i) {
    std::string topic;
    String::Deserialize(packet, topic);
    msg.subscribed_topics_.push_back(topic);
  }
  return msg;
}

}  // namespace farfler::network