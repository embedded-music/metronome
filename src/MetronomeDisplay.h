#pragma once

#include <cstdint>

#include "MetronomeState.h"

class MetronomeDisplay {
 public:
  void drawScreen(const MetronomeState& state, uint8_t currentBeat);
  void drawControl(const MetronomeState& state);
  void drawBeat(uint8_t beat, bool active);

 private:
  void drawInputStatus(ControlMode mode);
};
