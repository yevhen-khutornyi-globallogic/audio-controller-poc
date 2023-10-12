#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <charconv>

namespace vizio::s11n {

template<class Class, bool Arithmetic = std::is_arithmetic_v<typename Class::value_type>>
class Plain {
  using Class_ = std::remove_const_t<Class>; // Temporary workaround
public:
  Plain(Class_& obj) : ref { obj } {}
  Plain(const Class& obj) : ref { const_cast<Class_&>(obj) } {} // Temporary workaround
  std::string convert() const;
  bool convert(std::string_view str);
  bool convert(const std::string str) const {
    return convert(std::string_view{str});
  }
private:
  Class_& ref;
};

template<class Class>
class Plain<Class, true> {
  using Class_ = std::remove_const_t<Class>; // Temporary workaround
public:
  Plain(Class_& obj) : ref { obj } {}
  Plain(const Class& obj) : ref { const_cast<Class_&>(obj) } {}
  std::string convert() const {
    return std::to_string(ref.value);
  }
  bool convert(std::string_view str) const {
    typename Class::value_type value {};
    const auto res = std::from_chars(str.begin(), str.end(), value);
    if (res.ec == std::errc{} && Class::min <= value && value <= Class::max) {
      ref.value = value;
      return true;
    } else
      return false;
  }
  bool convert(const std::string str) const {
    return convert(std::string_view{str});
  }
private:
  Class_& ref;
};

template <class Class>
Plain(Class&) -> Plain<Class>;
template <class Class>
Plain(const Class&) -> Plain<Class>;
}
