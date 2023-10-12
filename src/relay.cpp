#include "relay.h"

namespace vizio::controller {
template<>
vizio::topic::audio::Volume Relay::transform(const vizio::sysreg::audio::Volume& volume) {
  return { volume.value };
}

template<>
vizio::topic::audio::Mute Relay::transform(const vizio::sysreg::audio::Mute& mute) {
  return {mute.value == mute.Enabled ? vizio::topic::audio::Mute::value_type::Enabled
                                     : vizio::topic::audio::Mute::value_type::Disabled};
}

const Relay::map Relay::items {
  make<vizio::sysreg::audio::Volume, vizio::topic::audio::Volume>(),
  make<vizio::sysreg::audio::Mute, vizio::topic::audio::Mute>(),
};

}
