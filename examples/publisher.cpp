#include <boost/asio.hpp>
#include <farfler/network/network.hpp>
#include <farfler/network/types.hpp>
#include <iostream>
#include <thread>

using namespace farfler::network;

int main() {
  // Create a network node
  boost::asio::io_context io_context;
  Network(io_context, "node");
  std::thread t([&io_context]() { io_context.run(); });

  // Subscribe to the "temperature" topic
  Network::Subscribe("temperature", [](const double &message) {
    std::cout << "Published temperature: " << message << std::endl;
  });

  while (true) {
    // Publish the temperature every second
    Network::Publish("temperature", 22.5);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}