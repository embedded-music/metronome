# Metronome

An audible, pocket-sized metronome for the M5StickC Plus2, using its two user
buttons, 135-by-240 display, and onboard passive buzzer. It provides adjustable
tempo and volume, a four-beat visual pulse, an accented downbeat, and selectable
click sounds.

The firmware grew from a musical-time experiment into a small standalone
instrument. Its reusable timing and pattern playback now live in published
packages, while this repository owns the M5StickC Plus2 interaction, display,
and click behavior.

## Controls

The control surface starts in Tempo mode. Button A increases and Button B
decreases the BPM. Short-click the Power button to cycle through:

- Tempo: A `+1 BPM`, B `-1 BPM`;
- Volume: A raises and B lowers the click level;
- Sound: A selects the accent sound and B selects the regular sound.

The next Power click returns to Tempo mode. The bottom of every screen shows
the current A/B assignments first, followed by the destination of the next
Power click (`PWR Volume`, `PWR Sounds`, or `PWR BPM`). The firmware currently
uses a fixed four-beat meter and begins playing immediately after boot.

```sh
just build
just upload
just monitor
just package-m5burner
```

`just upload-monitor` flashes and then opens the serial monitor. Run
`just devices` when the serial port is uncertain, and `just clean` to discard
the local PlatformIO build output.

## Application structure

The firmware keeps application concerns in local modules while their contracts
are still specific to this metronome:

- `BeatClock`: monotonic deadlines and phase-preserving interval changes;
- `ControlSurface`: Power screen changes and A/B release gestures;
- `MetronomeState`: bounded musical/control state and mode transitions;
- `MetronomeDisplay`: all screen layout and incremental beat drawing;
- `MetronomeAudio`: buzzer ownership, master gain, silent keep-alive, and PCM
  dispatch;
- `MetronomeClickSamples`: the fixed generated sound catalog;
- `main.cpp`: setup, event ordering, orchestration, and diagnostic logs.

These are internal boundaries, not reusable packages. Promotion waits for a
second real consumer to reveal a stable contract.

## Timing questions

The metronome is a small second consumer with which to investigate boundaries
also relevant to step sequencers and MIDI playback:

- tempo-independent musical position versus monotonic elapsed time;
- tempo as a conversion between quarter notes and real deadlines;
- meter and downbeat projection versus the underlying pulse;
- Start, Continue, Stop, and reset behavior;
- phase behavior when tempo changes during an interval;
- fractional intervals and long-term drift;
- catch-up after a delayed event loop;
- advancing position while suppressing stale audible clicks;
- independent audio and display consumers of the same timing event.

Standard MIDI Files and MIDI realtime messages are references rather than an
implementation template. SMF separates tick resolution, delta-timed events,
tempo, and time signature. MIDI Timing Clock uses 24 pulses per quarter note
and separate Start, Continue, and Stop messages. A file tick, a realtime MIDI
clock pulse, a sequencer step, and an audible metronome click are related but
not interchangeable concepts.

Useful references:

- [Standard MIDI Files](https://midi.org/standard-midi-files)
- [MIDI realtime messages](https://midi.org/about-midi-part-3midi-messages)

## Candidate boundary

The working vocabulary for the experiments is:

```text
musical position -- tempo --> real-time deadlines
        |
        +-- meter ----------> bar / beat / downbeat
        |
        +-- application ----> click, sequencer step, or timed event

transport ------------------> running state and cursor policy
```

This diagram is not yet an API. In particular, clock progression and output
dispatch may need different catch-up policies: a sequencer must preserve its
position, while a metronome should not emit a burst of clicks whose deadlines
have already passed.

`just package-m5burner` creates a versioned full-flash image and SHA-256
checksum under `dist/m5burner/`. See the
[M5Burner publishing guide](docs/publishing/m5burner.md).

## Possible next slices

- selectable meter and click grouping;
- Start, Stop, and Continue interaction;
- persisted tempo, volume, and sound choices.
