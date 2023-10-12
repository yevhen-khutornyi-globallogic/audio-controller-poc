#pragma once

#include <string>
#include <string_view>
#include <redox/client.hpp>
#include <vizio/s11n/plain.h>

namespace vizio::pubsub {
class Service {
public:
  Service() = default;
  Service(const Service&) = delete;
  Service(Service&) = delete;
  Service& operator=(const Service&) = delete;
  Service& operator=(Service&) = delete;
  bool connect(const std::string& redis_unix_socket) {
    return client.connectUnix(redis_unix_socket);
  }
  bool connect() { return connect(std::string{default_redis_socket}); }
  template<class Class>
  void publish(const Class& obj) {
    using S11N = typename Class::S11N;
    S11N s11n {obj};
    client.publish(std::string{Class::topic}, s11n.convert());
  }
  static constexpr std::string_view default_redis_socket = "/tmp/redis/redis.sock";
private:
  redox::Redox client {};
};
}
