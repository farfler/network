namespace farfler::network {

template <typename T, typename = void>
struct has_serialize : std::false_type {};

template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().Serialize())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_deserialize : std::false_type {};

template <typename T>
struct has_deserialize<T, std::void_t<decltype(T::Deserialize(
                              std::declval<std::vector<char>&>()))>>
    : std::true_type {};

template <typename T>
std::vector<char> Serialize(const T& message) {
  if constexpr (has_serialize<T>::value) {
    return message.Serialize();
  } else {
    std::vector<char> packet;
    packet.insert(packet.end(), reinterpret_cast<const char*>(&message),
                  reinterpret_cast<const char*>(&message) + sizeof(T));
    return packet;
  }
}

template <typename T>
T Deserialize(std::vector<char>& packet) {
  if constexpr (has_deserialize<T>::value) {
    return T::Deserialize(packet);
  } else {
    T msg;
    memcpy(&msg, packet.data(), sizeof(T));
    packet.erase(packet.begin(), packet.begin() + sizeof(T));
    return msg;
  }
}

}  // namespace farfler::network