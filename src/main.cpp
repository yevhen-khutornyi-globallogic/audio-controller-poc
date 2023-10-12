#include <signal.h>

#include <iostream> // XXX

#include "relay.h"

static void waitForTermination() {
  sigset_t sset;
  sigemptyset(&sset);
  sigaddset(&sset, SIGTERM);
  sigaddset(&sset, SIGINT);
  sigprocmask(SIG_BLOCK, &sset, NULL);
  int sig;
  sigwait(&sset, &sig);
}

int main() {
  vizio::controller::Relay retransmitter {};

  // XXX --------------------------
  vizio::sysreg::audio::Mute srmute { vizio::sysreg::audio::Mute::Disabled };
  vizio::sysreg::audio::Volume srvolume { 30 };
  vizio::topic::audio::Mute psmute { vizio::topic::audio::Mute::value_type::Disabled };
  vizio::topic::audio::Volume psvolume { 30 };

  std::cout << "Volume " << vizio::s11n::Plain{srvolume}.convert() << '\n';
  std::cout << "Volume " << vizio::s11n::Plain{psvolume}.convert() << '\n';
  std::cout << "Mute   " << vizio::s11n::Plain{srmute}.convert() << '\n';
  std::cout << "Mute   " << vizio::s11n::Plain{psmute}.convert() << '\n';
  // --------------------------

  waitForTermination();
  return 0;
}

