#pragma once
#include <string_view>
#include <vizio/s11n/plain.h>

// Topic Definitions for Audio
namespace vizio::topic::audio {

struct Mute {
  enum class value_type : bool {
    Disabled = false,
    Enabled = true,
  };
  value_type value;

  static constexpr std::string_view topic = "/ipc/hal/audio/operation/mute";
  using S11N = vizio::s11n::Plain<Mute>;
};

struct Volume {
  using value_type = int;
  value_type value;

  static constexpr std::string_view topic = "/ipc/hal/audio/operation/volume";
  static constexpr value_type min = 0;
  static constexpr value_type max = 100;
  using S11N =  vizio::s11n::Plain<Volume>;
};

} // namespace vizio::topic::audio

// Serialization specializations for Audio
namespace vizio::s11n {
template<>
class Plain<vizio::topic::audio::Mute, false> {
  using Class = vizio::topic::audio::Mute;
public:
  Plain(Class& obj) : ref { obj } {}
  Plain(const Class& obj) : ref { const_cast<Class&>(obj) } {}
  std::string convert() const {
    return std::string { ref.value == Class::value_type::Enabled ? true_ : false_};
  }
  bool convert(std::string_view str) const {
    if (str == true_)
      ref.value = Class::value_type::Disabled;
    else if (str == false_)
      ref.value = Class::value_type::Enabled;
    else
      return false;
    return true;
  }
  bool convert(const std::string str) const {
    return convert(std::string_view{str});
  }
private:
  Class& ref;
  static constexpr std::string_view true_ = "true";
  static constexpr std::string_view false_ = "false";
};

} // namespace vizio::s11n
