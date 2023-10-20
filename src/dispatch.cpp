#include <array>
#include <type_traits>

namespace vizio::audio {

namespace settings {

enum class AudioOut {
  Auto,
  ArcSoundBar,
  TvSpeakers,
  OpticalAnalog,
};

enum class eARC {
  Off,
  On,
};

enum class CEC {
  Disabled,
  Enabled,
};

}

namespace states {

enum class SoundBar {
  Connected,
  Disconnected,
};

enum class CEC {
  Connected,
  Disconnected,
};

enum class eARC {
  Connected,
  Disconnected,
};

}

namespace controller {

template<typename Enum>
constexpr Enum unspecified() noexcept {
  static_assert(std::is_enum_v<Enum>);
  return static_cast<Enum>(-1);
}

template<typename Enum>
constexpr bool match(Enum a, Enum b) noexcept  {
  static_assert(std::is_enum_v<Enum>);
  return a == b || a == unspecified<Enum>() || b == unspecified<Enum>();
}

struct Context {
  settings::AudioOut audioOut;
  settings::eARC earc;
  settings::CEC cec;
  states::SoundBar soundBar;
  constexpr bool operator==(const Context& c) const noexcept {
    return match(audioOut, c.audioOut)
        && match(soundBar, c.soundBar)
        && match(cec, c.cec)
        && match(earc, c.earc);
  }
};

class Dispatcher {
public:
  void operator()(Context);
private:
  void arc();
  void eArcPowerOnSB();         // Audio is heard from optical and ARC/eARC CEC power on sound bar
  void eArcPowerOffSB();        // Audio is heard from opticaln and eARC. CEC command to power off sound bar
  void eArcMuteSpeaker();       // Audio is heard from optical, eARC  No need to auto power on AMP. eARC amp unmute, speaker mute  void tvSpeakers();
  void tvSpeakers();            // Audio is heard from optical and TV speaker Force the digital mode as digital/PCM
  void tvSpeakersNoPowerOnSB(); // Audio is heard from optical and TV Speaker No CEC command to power on sound bar Force the digital mode as digital/PCM
  void tvSpeakersPowerOnSB();   // Audio is heard from optical and TV speaker CEC command to power on sound bar Force the digital mode as digital/PCM
  void tvSpeakersMuteArcSB();   // Audio is heard from optical and TV speaker eARC sound bar is muted TV speaker unmuted Force the digital mode as digital/PCM
  void tvSpeakersMuteSB();      // Audio is heard from optical and TV speaker Sound bar is muted?? Force the digital mode as digital/PCM
//  void tvSpeakersPowerOffSoundBar();
  void optical();               // Audio is heard from optical aARC sound bar is muted TV speaker is muted.
private:
  using Method = void(Dispatcher::*)();
  struct Item {
    Context context;
    Method method;
  };
  template<unsigned N>
  using Table = std::array<Item, N>;
  using AudioOut = settings::AudioOut;
  using CEC = settings::CEC;
  using eARC = settings::eARC;
  using SoundBar = states::SoundBar;

  static constexpr Table<16> table {{
    {{AudioOut::ArcSoundBar,   eARC::On,  CEC::Enabled,  SoundBar::Connected },    &Dispatcher::eArcPowerOnSB},         // Audio is heard from optical and ARC/eARC, CEC power on sound bar
    {{AudioOut::Auto,          eARC::On,  CEC::Enabled,  SoundBar::Disconnected }, &Dispatcher::tvSpeakers},            // Audio is heard from optical and TV speaker Force the digital mode as digital/PCM
    {{AudioOut::ArcSoundBar,   eARC::On,  CEC::Disabled, SoundBar::Connected },    &Dispatcher::eArcMuteSpeaker},       // Audio is heard from optical, eARC  No need to auto power on AMP. eARC amp unmute, speaker mute
    {{AudioOut::Auto,          eARC::On,  CEC::Disabled, SoundBar::Disconnected }, &Dispatcher::tvSpeakers},            // Audio is heard from optical and TV speaker Force the digital mode as digital/PCM
    //------------------------------------------------------------------------------------------------------------------
    {{AudioOut::ArcSoundBar,   eARC::Off, CEC::Enabled,  SoundBar::Connected },    &Dispatcher::eArcPowerOnSB},         // Audio is heard from optical and ARC, CEC command to power on sound bar if needed
    {{AudioOut::Auto,          eARC::Off, CEC::Enabled,  SoundBar::Disconnected }, &Dispatcher::tvSpeakers},            // Audio is heard from optical and TV speaker Force the digital mode as digital/PCM
    {{AudioOut::ArcSoundBar,   eARC::Off, CEC::Disabled, SoundBar::Connected },    &Dispatcher::tvSpeakersNoPowerOnSB}, // Audio is heard from optical and TV Speaker No CEC command to power on sound bar Force the digital mode as digital/PCM
    {{AudioOut::Auto,          eARC::Off, CEC::Disabled, SoundBar::Disconnected }, &Dispatcher::tvSpeakers},            // Audio is heard from optical and TV speaker Force the digital mode as digital/PCM
    //------------------------------------------------------------------------------------------------------------------
    {{AudioOut::TvSpeakers,    eARC::On,  CEC::Enabled,  unspecified<SoundBar>()}, &Dispatcher::tvSpeakersPowerOnSB},   // Audio is heard from optical and TV speaker CEC command to power on sound bar Force the digital mode as digital/PCM
    {{AudioOut::TvSpeakers,    eARC::On,  CEC::Disabled, unspecified<SoundBar>()}, &Dispatcher::tvSpeakersMuteArcSB},   // Audio is heard from optical and TV speaker eARC sound bar is muted TV speaker unmuted Force the digital mode as digital/PCM
    {{AudioOut::TvSpeakers,    eARC::Off, CEC::Enabled,  unspecified<SoundBar>()}, &Dispatcher::tvSpeakersNoPowerOnSB}, // Audio is heard from optical and TV Speaker Sound bar is not powered on (no CEC command to power on Sound bar) Force the digital mode as digital/PCM
    {{AudioOut::TvSpeakers,    eARC::Off, CEC::Disabled, unspecified<SoundBar>()}, &Dispatcher::tvSpeakersMuteSB},      // Audio is heard from optical and TV speaker Sound bar is muted?? Force the digital mode as digital/PCM
    //------------------------------------------------------------------------------------------------------------------
    {{AudioOut::OpticalAnalog, eARC::On,  CEC::Enabled,  unspecified<SoundBar>()}, &Dispatcher::eArcPowerOffSB},        // Audio is heard from opticaln and eARC. CEC command to power off sound bar
    {{AudioOut::OpticalAnalog, eARC::On,  CEC::Disabled, unspecified<SoundBar>()}, &Dispatcher::eArcMuteSpeaker},       // Audio is heard from optical and eARC eARC sound bar is unmuted TV speaker is muted.
    {{AudioOut::OpticalAnalog, eARC::Off, CEC::Enabled,  unspecified<SoundBar>()}, &Dispatcher::arc},                   // Audio is heard from optical and ARC
    {{AudioOut::OpticalAnalog, eARC::Off, CEC::Disabled, unspecified<SoundBar>()}, &Dispatcher::optical},               // Audio is heard from optical aARC sound bar is muted TV speaker is muted.
  }};
};


}

}
