#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace farfler::network {

template <typename T>
class Number {
 public:
  Number(T value = 0);
  static std::vector<char> Serialize(const T& msg);
  static std::vector<char> Serialize(const T& msg, std::vector<char>& packet);
  static T Deserialize(std::vector<char>& packet);
  static T Deserialize(std::vector<char>& packet, T& msg);

  operator T() const;

  T value;
};

using Int8 = Number<int8_t>;
using Int16 = Number<int16_t>;
using Int32 = Number<int32_t>;
using Int64 = Number<int64_t>;
using UInt8 = Number<uint8_t>;
using UInt16 = Number<uint16_t>;
using UInt32 = Number<uint32_t>;
using UInt64 = Number<uint64_t>;
using Float32 = Number<float>;
using Float64 = Number<double>;
using Boolean = Number<bool>;

class String {
 public:
  String(const std::string& value = "");
  static std::vector<char> Serialize(const std::string& msg);
  static std::vector<char> Serialize(const std::string& msg,
                                     std::vector<char>& packet);
  static std::string Deserialize(std::vector<char>& packet);
  static std::string Deserialize(std::vector<char>& packet, std::string& msg);

  operator std::string() const;

  std::string value;
};

class Vector2 {
 public:
  Vector2(float x = 0, float y = 0);
  static std::vector<char> Serialize(const Vector2& msg);
  static std::vector<char> Serialize(const Vector2& msg,
                                     std::vector<char>& packet);
  static Vector2 Deserialize(std::vector<char>& packet);
  static Vector2 Deserialize(std::vector<char>& packet, Vector2& msg);

  float x, y;
};

class Vector3 {
 public:
  Vector3(float x = 0, float y = 0, float z = 0);
  static std::vector<char> Serialize(const Vector3& msg);
  static std::vector<char> Serialize(const Vector3& msg,
                                     std::vector<char>& packet);
  static Vector3 Deserialize(std::vector<char>& packet);
  static Vector3 Deserialize(std::vector<char>& packet, Vector3& msg);

  float x, y, z;
};

class Coordinate {
 public:
  Coordinate(double latitude = 0, double longitude = 0);
  static std::vector<char> Serialize(const Coordinate& msg);
  static std::vector<char> Serialize(const Coordinate& msg,
                                     std::vector<char>& packet);
  static Coordinate Deserialize(std::vector<char>& packet);
  static Coordinate Deserialize(std::vector<char>& packet, Coordinate& msg);

  double latitude, longitude;
};

}  // namespace farfler::network
