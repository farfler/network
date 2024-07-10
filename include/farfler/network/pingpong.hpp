#pragma once

#include <farfler/network/types.hpp>
#include <string>
#include <vector>

namespace farfler::network {

class UdpPing {
 public:
  UdpPing(const std::string& id = "", const std::string& name = "",
          const std::string& udp_address = "", uint16_t udp_port = 0,
          const std::string& tcp_address = "", uint16_t tcp_port = 0);

  static std::vector<char> Serialize(const UdpPing& msg);
  static std::vector<char> Serialize(const UdpPing& msg,
                                     std::vector<char>& packet);
  static UdpPing Deserialize(std::vector<char>& packet);
  static UdpPing Deserialize(std::vector<char>& packet, UdpPing& msg);

  std::string id_;
  std::string name_;
  std::string udp_address_;
  uint16_t udp_port_;
  std::string tcp_address_;
  uint16_t tcp_port_;
};

class UdpPong {
 public:
  UdpPong(const std::string& id = "", const std::string& name = "",
          const std::string& udp_address = "", uint16_t udp_port = 0,
          const std::string& tcp_address = "", uint16_t tcp_port = 0);

  static std::vector<char> Serialize(const UdpPong& msg);
  static std::vector<char> Serialize(const UdpPong& msg,
                                     std::vector<char>& packet);
  static UdpPong Deserialize(std::vector<char>& packet);
  static UdpPong Deserialize(std::vector<char>& packet, UdpPong& msg);

  std::string id_;
  std::string name_;
  std::string udp_address_;
  uint16_t udp_port_;
  std::string tcp_address_;
  uint16_t tcp_port_;
};

class TcpPing {
 public:
  TcpPing(const std::string& id = "", const std::string& name = "",
          const std::string& udp_address = "", uint16_t udp_port = 0,
          const std::string& tcp_address = "", uint16_t tcp_port = 0);

  static std::vector<char> Serialize(const TcpPing& msg);
  static std::vector<char> Serialize(const TcpPing& msg,
                                     std::vector<char>& packet);
  static TcpPing Deserialize(std::vector<char>& packet);
  static TcpPing Deserialize(std::vector<char>& packet, TcpPing& msg);

  std::string id_;
  std::string name_;
  std::string udp_address_;
  uint16_t udp_port_;
  std::string tcp_address_;
  uint16_t tcp_port_;
  std::vector<std::string> subscribed_topics_;
};

class TcpPong {
 public:
  TcpPong(const std::string& id = "", const std::string& name = "",
          const std::string& udp_address = "", uint16_t udp_port = 0,
          const std::string& tcp_address = "", uint16_t tcp_port = 0);

  static std::vector<char> Serialize(const TcpPong& msg);
  static std::vector<char> Serialize(const TcpPong& msg,
                                     std::vector<char>& packet);
  static TcpPong Deserialize(std::vector<char>& packet);
  static TcpPong Deserialize(std::vector<char>& packet, TcpPong& msg);

  std::string id_;
  std::string name_;
  std::string udp_address_;
  uint16_t udp_port_;
  std::string tcp_address_;
  uint16_t tcp_port_;
  std::vector<std::string> subscribed_topics_;
};

}  // namespace farfler::network