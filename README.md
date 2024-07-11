<h1 align="center">Farfler&trade; Network</h1>

Farfler Network is a C++ library that implements a distributed publish-subscribe system using UDP for discovery and TCP for reliable communication. It allows nodes to discover each other, establish connections, and exchange messages on various topics.

<h2 id="getting-started">Getting Started</h2>

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

<p align="center">
  Made with ❤️ by <a href="https://github.com/danielbacsur" target="_blank">Daniel Bacsur</a>
</p>
