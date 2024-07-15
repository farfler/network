<h1 align="center">Farfler&trade; Network</h1>

<p align="center">
  <a href="#getting-started">Getting Started</a> &#xa0; | &#xa0;
  <a href="#subscribing">Subscribing</a> &#xa0; | &#xa0;
  <a href="#publishing">Publishing</a> &#xa0; | &#xa0;
  <a href="#multiple-network-instances">Multiple Instances</a> &#xa0; | &#xa0;
  <a href="#author">Author</a>
</p>

Farfler Network is a C++ library that implements a distributed publish-subscribe system using UDP for discovery and TCP for reliable communication. It allows nodes to discover each other, establish connections, and exchange messages on various topics.

<h2 id="getting-started">Getting Started</h2>

To install Farfler Network, you need to clone the repository and build the library using CMake. Here's how you can do it:

```bash
git clone https://github.com/farfler/network
cd network
mkdir build
cd build
cmake .. && make
```

To begin using Farfler Network, you need to set up the basic network infrastructure. This involves creating an IO context, initializing the network with a node name, and running the IO context in a separate thread. Here's how to set up the basic environment:

```cxx
#include <boost/asio.hpp>
#include <farfler/network/network.hpp>
#include <thread>

int main() {
  boost::asio::io_context io_context;
  Network(io_context, "node");
  std::thread t([&io_context]() { io_context.run(); });

  ...

  io_context.stop();
  t.join();
  return 0;
}
```

<h2 id="subscribing">Subscribing</h2>

Farfler Network offers flexible subscription options. You can subscribe to receive messages from all nodes with `Subscribe`, only from remote nodes with `SubscribeOnline`, or only from the local node with `SubscribeOffline`. This allows for fine-grained control over message reception. Here are examples of all three subscription types:

```cxx
Network::Subscribe("temperature", [](const double &message) {
  std::cout << "Received temperature from any node: " << message << std::endl;
});

Network::SubscribeOnline("status", [](const bool &message) {
  std::cout << "Received online status from remote node: " << message << std::endl;
});

Network::SubscribeOffline("position", [](const Vector3 &message) {
  std::cout << "Received local position: (" << message.x << ", " << message.y << ", " << message.z << ")" << std::endl;
});
```

<h2 id="publishing">Publishing</h2>

Similarly, you can publish messages to all subscribed nodes with `Publish`, specifically to remote nodes with `PublishOnline`, or only to the local node with `PublishOffline`.

```cxx
Network::Publish("temperature", 22.5);

Network::PublishOnline("status", true);

Network::PublishOffline("position", Vector3(1.0, 2.0, 3.0));
```

<h2 id="multiple-network-instances">Multiple Network Instances</h2>

The library supports the creation and management of multiple independent network instances within a single application. This feature is particularly useful for scenarios where you need to isolate different parts of your network communication or manage distinct network configurations. Here's how you can work with multiple network instances:

```cxx
// Create multiple network instances
Network network1(io_context, "network1");
Network network2(io_context, "network2");

// Publish to specific networks
Network::Publish(network1, "temperature", 22.5);
Network::Publish(network2, "humidity", 45.0);

// Subscribe to topics on specific networks
Network::Subscribe(network1, "temperature", [](const double &temp) {
  std::cout << "Network1 temperature: " << temp << " °C" << std::endl;
});

Network::Subscribe(network2, "humidity", [](const double &humidity) {
  std::cout << "Network2 humidity: " << humidity << "%" << std::endl;
});
```

<h2 id="singleton-instance-fallback">Singleton Instance Fallback</h2>

Farfler Network implements a convenient singleton instance fallback mechanism. This feature allows you to use network operations without explicitly specifying a network instance, defaulting to the first created network instance. Here's how it works:

```cxx
// Create multiple network instances
Network network1(io_context, "network1");
Network network2(io_context, "network2");

// The first created network (network1) becomes the default singleton instance

// These operations will use network1 by default
Network::Publish("temperature", 22.5);
Network::Subscribe("humidity", [](const double &humidity) {
  std::cout << "Humidity: " << humidity << "%" << std::endl;
});

// You can still explicitly specify a network when needed
Network::Publish(network2, "pressure", 1013.25);
```

This mechanism allows you to write cleaner code when working with a single network or when one network is predominantly used, while still maintaining the ability to manage multiple networks when necessary.

<p id="author" align="center">
  Made with ❤️ by <a href="https://github.com/danielbacsur" target="_blank">Daniel Bacsur</a>
</p>
