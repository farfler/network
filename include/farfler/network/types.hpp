#pragma once

#include <cstdint>
#include <vector>

namespace farfler::network {

template <typename T>
class Number {
 public:
  explicit Number(T value = 0);
  static std::vector<char> Serialize(const T& msg);
  static std::vector<char> Serialize(const T& msg, std::vector<char>& packet);
  static T Deserialize(std::vector<char>& packet);
  static T Deserialize(std::vector<char>& packet, T& msg);

  T value_;
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

}  // namespace farfler::network
