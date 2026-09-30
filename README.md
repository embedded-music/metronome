# Pocket Metronome

Pocket Metronome is a visual and audible metronome for the M5StickC Plus2. It
uses the built-in display, buttons, and passive buzzer, so no additional
hardware is required.

The firmware provides:

- adjustable tempo from 30 to 300 BPM;
- a four-beat visual pulse with a distinct downbeat;
- separate accent and regular click sounds;
- adjustable buzzer volume;
- immediate playback from startup;
- phase-preserving tempo changes;
- suppression of stale clicks when the event loop is delayed.

## Controls

The firmware opens on the BPM screen. Short-click the Power button to cycle
through BPM, Volume, and Sounds, then return to BPM.

| Screen | Button A | Button B | Power click |
| --- | --- | --- | --- |
| BPM | +1 BPM | -1 BPM | Volume |
| Volume | Louder | Quieter | Sounds |
| Sounds | Next accent | Next regular click | BPM |

The footer always shows the current A/B actions followed by the destination of
the next Power click. A long Power hold retains the device's normal power
behavior.

## Build and flash

The project uses PlatformIO with the Arduino framework and pioarduino's stable
ESP32 platform.

```sh
just build
just upload
just monitor
```

`just upload-monitor` flashes the firmware and opens the serial monitor.
`just devices` lists available serial devices, and `just clean` removes the
local PlatformIO build output.

The application resolves its reusable components from the PlatformIO Registry:

- `fcz2/m5-tone-output@0.1.5`;
- `fcz2/step-trigger@0.2.0`;
- their published `monophonic-instrument`, `musical-clock`, and
  `firmware-contracts` dependencies.

It does not require sibling repository checkouts.

## Architecture

The metronome represents one bar as a four-step `TriggerPattern`: a strong
first step followed by three normal steps. `TriggerPatternPlayer` owns the
pattern cursor and its `AlternatingDeadlineClock`, advances from monotonic
microsecond timestamps, and emits trigger events for audible beats.

The application remains responsible for translating that shared playback
machinery into a metronome:

- `ControlSurface` maps short Power clicks and A/B releases to commands;
- `MetronomeState` owns tempo, volume, screen mode, and sound selections;
- `MetronomeDisplay` draws controls and the four-beat pulse;
- `MetronomeAudio` owns the passive-buzzer output and keep-alive policy;
- `MetronomeClickSamples` builds the selectable PCM click catalog;
- `main.cpp` composes the pattern source, player, trigger sink, UI, and audio.

Tempo changes preserve the fraction of the current beat interval that remains.
If polling arrives late, musical position catches up but missed clicks are not
replayed as an audio burst.

## M5Burner package

`VERSION` is the firmware release version. Generate the merged full-flash image
and checksum with:

```sh
just package-m5burner
```

The artifacts are written under `dist/m5burner/`. The `.bin` contains the
bootloader, partition table, Arduino boot metadata, and application and is
intended to be flashed at address `0x0`.

See the [M5Burner publishing guide](docs/publishing/m5burner.md) for the
hardware release gate, upload steps, and suggested directory metadata.

## Development history

The project is developed in small, hardware-testable slices. Narrative records
of the timing experiments, audio probes, shared-package extractions, UI
decisions, and release work live in [`docs/devlog`](docs/devlog/).

Possible future slices include selectable meter and click grouping, transport
controls, and persisted tempo, volume, and sound choices.
