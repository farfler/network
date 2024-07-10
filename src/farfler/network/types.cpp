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

}  // namespace farfler::network
