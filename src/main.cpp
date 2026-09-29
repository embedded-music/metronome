#include <Arduino.h>
#include <esp_timer.h>
#include <M5Unified.h>

#include "ControlSurface.h"
#include "MetronomeAudio.h"
#include "MetronomeClickSamples.h"
#include "MetronomeDisplay.h"
#include "MetronomeState.h"
#include "TriggerPatternLoader.h"
#include "TriggerPatternPlayer.h"

namespace {
constexpr uint32_t SERIAL_BAUD = 115200;

MetronomeState state;
ControlSurface controls;
MetronomeAudio audio;
MetronomeDisplay display;

class MetronomePatternSource : public TriggerPatternSource {
 public:
  bool load() {
    static constexpr TriggerPatternCell cells[] = {
        {0, 0, StepLevel::Strong},
        {0, 1, StepLevel::Normal},
        {0, 2, StepLevel::Normal},
        {0, 3, StepLevel::Normal},
    };
    return TriggerPatternLoader::load(pattern_, MetronomeState::BEATS_PER_BAR,
                                      cells);
  }

  const TriggerPattern& currentPattern() const override { return pattern_; }

 private:
  TriggerPattern pattern_;
};

class MetronomeClickSink : public TriggerEventSink {
 public:
  void trigger(const TriggerEvent& event) override {
    const bool accent = event.level == StepLevel::Strong;
    const ClickSoundId sound =
        accent ? state.accentClick() : state.regularClick();
    const PcmS8Sample& sample = clickSoundSample(sound);
    const bool started = audio.play(sample);
    const uint32_t durationMs =
        sample.sampleCount * 1000UL / sample.sampleRateHz;
    Serial.printf(
        "click: accent=%s sound=%s duration_ms=%lu "
        "sample_rate_hz=%lu samples=%u volume=%u gain=master "
        "playback=started ok=%s now_ms=%lu\n",
        accent ? "yes" : "no", clickSoundLogName(sound),
        static_cast<unsigned long>(durationMs),
        static_cast<unsigned long>(sample.sampleRateHz),
        static_cast<unsigned>(sample.sampleCount), state.clickVolume(),
        started ? "yes" : "no", static_cast<unsigned long>(millis()));
  }

};

MetronomePatternSource patternSource;
MetronomeClickSink clickSink;
TriggerPatternPlayer player(patternSource, clickSink);

StepIntervals currentStepIntervals() {
  return StepIntervals::constant(state.beatIntervalMs() * 1000ULL);
}

void adjustTempo(int8_t direction, uint64_t nowUs, uint32_t nowMs) {
  const uint16_t previousTempo = state.tempoBpm();
  if (!state.adjustTempo(direction)) {
    Serial.printf(
        "control: action=adjust mode=tempo direction=%s previous=%u "
        "value=%u interval_ms=%lu phase=unchanged clamped=yes now_ms=%lu\n",
        direction > 0 ? "up" : "down", previousTempo, state.tempoBpm(),
        static_cast<unsigned long>(state.beatIntervalMs()),
        static_cast<unsigned long>(nowMs));
    return;
  }

  const bool rescheduled = player.changeTiming(
      nowUs, currentStepIntervals(), TriggerTimingChange::PreservePhase);
  if (!rescheduled) {
    Serial.println("clock: reschedule_failed");
    return;
  }
  display.drawControl(state);
  Serial.printf(
      "control: action=adjust mode=tempo direction=%s previous=%u value=%u "
      "interval_ms=%lu phase=preserved now_ms=%lu\n",
      direction > 0 ? "up" : "down", previousTempo, state.tempoBpm(),
      static_cast<unsigned long>(state.beatIntervalMs()),
      static_cast<unsigned long>(nowMs));
}

void adjustVolume(int8_t direction) {
  const uint8_t previousVolume = state.clickVolume();
  if (!state.adjustVolume(direction)) return;

  audio.setVolume(state.clickVolume());
  display.drawControl(state);
  Serial.printf(
      "control: action=adjust mode=volume direction=%s previous=%u value=%u\n",
      direction > 0 ? "up" : "down", previousVolume, state.clickVolume());
}

void selectNextSound(bool accent) {
  state.selectNextSound(accent);
  display.drawControl(state);
  const ClickSoundId selection =
      accent ? state.accentClick() : state.regularClick();
  Serial.printf("control: action=adjust mode=sound role=%s selected=%s\n",
                accent ? "accent" : "regular",
                clickSoundLogName(selection));
}

void applyControl(ControlCommand command, uint64_t nowUs, uint32_t nowMs) {
  if (command == ControlCommand::None) return;

  if (command == ControlCommand::NextMode) {
    Serial.println("button: name=power action=clicked");
    state.cycleControlMode();
    display.drawControl(state);
    Serial.printf("control: action=mode selected=%s\n",
                  state.controlModeName());
    return;
  }

  const int8_t direction =
      command == ControlCommand::Increase ? 1 : -1;
  Serial.printf("button: name=%c action=released command=%s\n",
                direction > 0 ? 'a' : 'b',
                direction > 0 ? "increase" : "decrease");

  switch (state.controlMode()) {
    case ControlMode::Tempo:
      adjustTempo(direction, nowUs, nowMs);
      break;
    case ControlMode::Volume:
      adjustVolume(direction);
      break;
    case ControlMode::Sound:
      selectNextSound(direction > 0);
      break;
  }
}

void advanceVisibleBeat(uint64_t nowUs, uint32_t nowMs) {
  const TriggerPatternPlayerUpdate update = player.update(nowUs);
  if (!update.advanced()) return;

  if (update.elapsedSteps > 1) {
    Serial.printf("player: skipped_steps count=%lu\n",
                  static_cast<unsigned long>(update.elapsedSteps));
  }
  display.drawBeat(update.previousStep, false);
  display.drawBeat(update.currentStep, true);

  Serial.printf("beat: index=%u elapsed=%lu now_ms=%lu\n",
                update.currentStep + 1,
                static_cast<unsigned long>(update.elapsedSteps),
                static_cast<unsigned long>(nowMs));
}
}  // namespace

void setup() {
  auto config = M5.config();
  config.internal_spk = true;
  config.internal_mic = false;
  config.fallback_board = m5::board_t::board_M5StickCPlus2;
  M5.begin(config);

  Serial.begin(SERIAL_BAUD);
  delay(200);

  M5.Display.setRotation(1);
  buildMetronomeClickSamples();
  const bool keepAliveStarted = audio.begin(state.clickVolume());
  const bool patternLoaded = patternSource.load();
  display.drawScreen(state, player.currentStep());
  const bool playerStarted = player.begin(
      static_cast<uint64_t>(esp_timer_get_time()), currentStepIntervals(),
      TriggerStart::EmitImmediately);
  if (!patternLoaded || !playerStarted) {
    Serial.println("player: begin_failed");
  }
  Serial.printf(
      "metronome: audible_clock=ready bpm=%u volume=%u beats_per_bar=%u "
      "control_mode=%s\n",
      state.tempoBpm(), state.clickVolume(), MetronomeState::BEATS_PER_BAR,
      state.controlModeName());
  Serial.printf(
      "audio: idle_policy=keep_alive ok=%s\n",
      keepAliveStarted ? "yes" : "no");
}

void loop() {
  M5.update();
  const uint64_t nowUs = static_cast<uint64_t>(esp_timer_get_time());
  const uint32_t nowMs = millis();
  advanceVisibleBeat(nowUs, nowMs);
  applyControl(controls.poll(), nowUs, nowMs);
  delay(1);
}
