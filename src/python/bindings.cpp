#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <boost/asio.hpp>
#include <farfler/network/network.hpp>
#include <farfler/network/types.hpp>
#include <thread>

namespace py = pybind11;
using namespace farfler::network;

class PyIOContext {
public:
  PyIOContext() : io_context_(), work_(boost::asio::make_work_guard(io_context_)) {
    thread_ = std::thread([this]() { this->io_context_.run(); });
  }
  
  ~PyIOContext() {
    stop();
    if (thread_.joinable()) {
      thread_.join();
    }
  }
  
  void stop() {
    io_context_.stop();
  }
  
  boost::asio::io_context& get_io_context() {
    return io_context_;
  }

private:
  boost::asio::io_context io_context_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_;
  std::thread thread_;
};

class PyNetwork {
public:
  PyNetwork(PyIOContext& io_context, const std::string& name)
    : network_(io_context.get_io_context(), name) {}


  template<typename T>
  static void publish(const std::string& topic, const T& message) {
    Network::Publish(topic, message);
  }

  static void publish_string(const std::string& topic, const std::string& message) {
    Network::Publish(topic, String(message));
  }

  template<typename T>
  static void subscribe(const std::string& topic, py::function callback) {
    Network::Subscribe(topic, [callback](const T& message) {
      py::gil_scoped_acquire acquire;
      callback(message);
    });
  }

private:
  Network network_;
};

PYBIND11_MODULE(farfler_network, m) {
  py::class_<PyIOContext>(m, "IOContext")
      .def(py::init<>())
      .def("stop", &PyIOContext::stop);

  py::class_<PyNetwork>(m, "Network")
      .def(py::init<PyIOContext&, const std::string&>())

      .def_static("publish_float", &PyNetwork::publish<double>)
      .def_static("subscribe_float", &PyNetwork::subscribe<double>)

      .def_static("publish_int", &PyNetwork::publish<int>)
      .def_static("subscribe_int", &PyNetwork::subscribe<int>)

      .def_static("publish_string", &PyNetwork::publish_string)
      .def_static("subscribe_string", &PyNetwork::subscribe<std::string>) // TODO: Fix numap_chunk error
      
      .def_static("publish_bool", &PyNetwork::publish<bool>)
      .def_static("subscribe_bool", &PyNetwork::subscribe<bool>)
      
      .def_static("publish_vector3", &PyNetwork::publish<Vector3>)
      .def_static("subscribe_vector3", &PyNetwork::subscribe<Vector3>);
      
  py::class_<Vector3>(m, "Vector3")
      .def(py::init<float, float, float>())
      .def_readwrite("x", &Vector3::x)
      .def_readwrite("y", &Vector3::y)
      .def_readwrite("z", &Vector3::z);
}
