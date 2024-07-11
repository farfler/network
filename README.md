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

<p align="center">
  Made with ❤️ by <a href="https://github.com/danielbacsur" target="_blank">Daniel Bacsur</a>
</p>
