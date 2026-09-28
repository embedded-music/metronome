# 013 — Compose the metronome with the trigger pattern player

## Goal

Test whether the player extracted for the electronic drummer is a useful
musical boundary rather than a drummer-specific convenience.

## Design

The four-beat bar is now a four-step, one-lane `TriggerPattern`: the first cell
is strong and the remaining cells are normal. A small trigger sink maps those
levels to the user-selected accent and regular click samples.

`TriggerPatternPlayer` now owns the deadline clock and cursor. The application
calculates the beat interval, preserves phase when tempo changes, redraws from
the player's update result, and retains ownership of controls, samples, and
display. `MetronomeState` no longer stores a duplicate current-beat cursor.
Startup explicitly chooses `EmitImmediately`, preserving the original opening
downbeat without a separate manual trigger call.
Its schedule is expressed as `StepIntervals::constant(...)`, matching the
metronome's single equal beat interval.

Late polling retains the prior policy: the cursor catches up visually, but a
missed click is not replayed as an audio burst.

## Validation

```text
pio run
```

The M5StickC Plus2 firmware builds successfully against the local
`step-trigger` 0.2.0 candidate. Hardware validation should confirm the opening
accent, four-beat display cycle, stable tempo changes, and no audible catch-up
bursts.
