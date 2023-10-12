#pragma once
#include <sysreg_wrap.h>
#include <vizio/sysreg/audio.h>
#include <vizio/topic/audio.h>
#include <vizio/s11n/plain.h>
#include <vizio/pubsub.h>

#include <chrono>
#include <functional>
#include <string_view>
#include <string>

namespace vizio::controller {
class Relay {
public:
  Relay() {
    subscribe();
    publisher.connect(); // TODO throw if failed
  }
  Relay(const Relay&) = delete;
  Relay(Relay&&) = delete;
  Relay& operator=(const Relay&) = delete;
  Relay& operator=(Relay&&) = delete;

private: // Pubsub side
  using map = std::unordered_map<std::string_view, bool (*)(Relay&, std::string_view value)>;
  using item = map::value_type;

  // transforms values between domains Sysreg and Pubsub
  template<class ToPubsub, class FromSysreg>
  static ToPubsub transform(const FromSysreg&);

  // Convert received value and publish
  template<class FromSysreg, class ToPubsub>
  static bool convertAndPublish(Relay& thiz, std::string_view strval) {
    using Deserializer = typename FromSysreg::S11N;
    FromSysreg value{};
    Deserializer deserializer{ value };
    if (deserializer.convert(strval)) {
      thiz.publisher.publish(Relay::transform<ToPubsub>(value));
    } else {
      return false;
    }
    return true;
  }

  template<class FromSysreg, class ToPubsub>
  static item make() {
    return {
      FromSysreg::key,
      &convertAndPublish<FromSysreg, ToPubsub>
    };
  }
private: // Sysreg side
  bool changed(const std::string& key) {
    using namespace std::literals;
    auto [val, ok] = sysreg.get(key, ++ipcseq, 100ms);
    if (ok) {
      const auto found = items.find(std::string_view{key});
      if (found == items.end()) {
        // TODO log("Unexpected key", key);
      } else {
        found->second(*this, std::string_view{val});
      }
    }
    return true;
  }
  bool deleted(const std::string& key) {
    // TODO what?
    return true;
  }
  bool dispatch(const std::string& key, sysreg_event_t event) {
    return event == SYSREG_EVENT_DELETE ? deleted(key) : changed(key);
  }

  auto callback() {
    return std::bind(&Relay::dispatch, this, std::placeholders::_1, std::placeholders::_2);
  }
  void subscribe() {
    for (const auto& i : items) {
      auto ok = sysreg.subscribe(i.first.data(), callback());
      (void) ok; // TODO What if subscription failed?
    }
  }

private:
  static const map items;
  vizio::pubsub::Service publisher {};
  vizio::systemregistry::SystemRegistry sysreg{vizio::ipc::Broker::getDefaultBroker()};
  vizio::ipc::SequenceID ipcseq {};
};
} // namespace vizio::controller
