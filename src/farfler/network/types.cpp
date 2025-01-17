#include <cstring>
#include <farfler/network/types.hpp>

namespace farfler::network {

template <typename T>
Number<T>::Number(T value) : value(value) {}

template <typename T>
std::vector<char> Number<T>::Serialize(const T& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

template <typename T>
std::vector<char> Number<T>::Serialize(const T& msg,
                                       std::vector<char>& packet) {
  packet.insert(packet.end(), reinterpret_cast<const char*>(&msg),
                reinterpret_cast<const char*>(&msg) + sizeof(T));
  return packet;
}

template <typename T>
T Number<T>::Deserialize(std::vector<char>& packet) {
  T msg;
  Deserialize(packet, msg);
  return msg;
}

template <typename T>
T Number<T>::Deserialize(std::vector<char>& packet, T& msg) {
  memcpy(&msg, packet.data(), sizeof(T));
  packet.erase(packet.begin(), packet.begin() + sizeof(T));
  return msg;
}

template <typename T>
Number<T>::operator T() const {
    return value;
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

String::String(const std::string& value) : value(value) {}

std::vector<char> String::Serialize(const std::string& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> String::Serialize(const std::string& msg,
                                    std::vector<char>& packet) {
  UInt32::Serialize(msg.size(), packet);
  packet.insert(packet.end(), msg.begin(), msg.end());
  return packet;
}

std::string String::Deserialize(std::vector<char>& packet) {
  std::string msg;
  Deserialize(packet, msg);
  return msg;
}

std::string String::Deserialize(std::vector<char>& packet, std::string& msg) {
  uint32_t size = UInt32::Deserialize(packet);
  msg = std::string(packet.begin(), packet.begin() + size);
  packet.erase(packet.begin(), packet.begin() + size);
  return msg;
}

String::operator std::string() const {
    return value;
}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

std::vector<char> Vector2::Serialize(const Vector2& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> Vector2::Serialize(const Vector2& msg,
                                     std::vector<char>& packet) {
  Float32::Serialize(msg.x, packet);
  Float32::Serialize(msg.y, packet);
  return packet;
}

Vector2 Vector2::Deserialize(std::vector<char>& packet) {
  Vector2 msg;
  Deserialize(packet, msg);
  return msg;
}

Vector2 Vector2::Deserialize(std::vector<char>& packet, Vector2& msg) {
  Float32::Deserialize(packet, msg.x);
  Float32::Deserialize(packet, msg.y);
  return msg;
}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

std::vector<char> Vector3::Serialize(const Vector3& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> Vector3::Serialize(const Vector3& msg,
                                     std::vector<char>& packet) {
  Float32::Serialize(msg.x, packet);
  Float32::Serialize(msg.y, packet);
  Float32::Serialize(msg.z, packet);
  return packet;
}

Vector3 Vector3::Deserialize(std::vector<char>& packet) {
  Vector3 msg;
  Deserialize(packet, msg);
  return msg;
}

Vector3 Vector3::Deserialize(std::vector<char>& packet, Vector3& msg) {
  Float32::Deserialize(packet, msg.x);
  Float32::Deserialize(packet, msg.y);
  Float32::Deserialize(packet, msg.z);
  return msg;
}

Coordinate::Coordinate(double latitude, double longitude)
    : latitude(latitude), longitude(longitude) {}

std::vector<char> Coordinate::Serialize(const Coordinate& msg) {
  std::vector<char> packet;
  Serialize(msg, packet);
  return packet;
}

std::vector<char> Coordinate::Serialize(const Coordinate& msg,
                                        std::vector<char>& packet) {
  Float64::Serialize(msg.latitude, packet);
  Float64::Serialize(msg.longitude, packet);
  return packet;
}

Coordinate Coordinate::Deserialize(std::vector<char>& packet) {
  Coordinate msg;
  Deserialize(packet, msg);
  return msg;
}

Coordinate Coordinate::Deserialize(std::vector<char>& packet, Coordinate& msg) {
  Float64::Deserialize(packet, msg.latitude);
  Float64::Deserialize(packet, msg.longitude);
  return msg;
}

}  // namespace farfler::network
