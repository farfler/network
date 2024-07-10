#include <cstring>
#include <farfler/network/types.hpp>

namespace farfler::network {

template <typename T>
Number<T>::Number(T value) : value_(value) {}

template <typename T>
std::vector<char> Number<T>::Serialize(const T &msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

template <typename T>
std::vector<char> Number<T>::Serialize(const T &msg,
                                       std::vector<char> &packet) {
  packet.insert(packet.end(), reinterpret_cast<const char *>(&msg),
                reinterpret_cast<const char *>(&msg) + sizeof(T));
  return packet;
}

template <typename T>
T Number<T>::Deserialize(std::vector<char> &packet) {
  T msg;
  Deserialize(packet, msg);
  return msg;
}

template <typename T>
T Number<T>::Deserialize(std::vector<char> &packet, T &msg) {
  memcpy(&msg, packet.data(), sizeof(T));
  packet.erase(packet.begin(), packet.begin() + sizeof(T));
  return msg;
}

template class Number<int8_t>;
template class Number<int16_t>;
template class Number<int32_t>;
template class Number<int64_t>;
template class Number<uint8_t>;
template class Number<uint16_t>;
template class Number<uint32_t>;
template class Number<uint64_t>;
template class Number<float>;
template class Number<double>;
template class Number<bool>;

String::String(const std::string &value) : value_(value) {}

std::vector<char> String::Serialize(const std::string &msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> String::Serialize(const std::string &msg,
                                    std::vector<char> &packet) {
  UInt32::Serialize(msg.size(), packet);
  packet.insert(packet.end(), msg.begin(), msg.end());
  return packet;
}

std::string String::Deserialize(std::vector<char> &packet) {
  std::string msg;
  Deserialize(packet, msg);
  return msg;
}

std::string String::Deserialize(std::vector<char> &packet, std::string &msg) {
  uint32_t size = UInt32::Deserialize(packet);
  msg = std::string(packet.begin(), packet.begin() + size);
  packet.erase(packet.begin(), packet.begin() + size);
  return msg;
}

}  // namespace farfler::network
