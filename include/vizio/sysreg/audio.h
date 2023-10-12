#pragma once
#include <string_view>
#include <vizio/s11n/plain.h>

// System Registry Key Definitions for Audio
namespace vizio::sysreg::audio {

struct Mute {
  enum value_type {
    Disabled = 0,
    Enabled = 1,
  };
  value_type value;

  static constexpr std::string_view key = "/current/mute";
  using S11N =  vizio::s11n::Plain<Mute>;
};

struct Volume {
  using value_type = int;
  value_type value;

  static constexpr std::string_view key = "/current/volume";
  static constexpr value_type min = 0;
  static constexpr value_type max = 100;
  using S11N =  vizio::s11n::Plain<Volume>;
};

} // namespace vizio::sysreg::audio

// Serialization specializations for Audio
namespace vizio::s11n {
template<>
class Plain<vizio::sysreg::audio::Mute, false> {
  using Class = vizio::sysreg::audio::Mute;
public:
  Plain(Class& obj) : ref { obj } {}
  std::string convert() const {
    return std::to_string(ref.value);
  }
  bool convert(std::string_view str) const {
    if (str == zero)
      ref.value = Class::Disabled;
    else if (str == one)
      ref.value = Class::Enabled;
    else
      return false;
    return true;
  }
  bool convert(const std::string str) const {
    return convert(std::string_view{str});
  }
private:
  Class& ref;
  static constexpr std::string_view zero = "0";
  static constexpr std::string_view one = "1";
};

} // namespace vizio::s11n
