# 019 — Rewrite the README around the current product

## Goal

Replace the experiment-era repository overview with accurate documentation for
the M5Burner-ready firmware.

## Change

The README now leads with Pocket Metronome's capabilities and complete control
map. It documents published dependencies, the full-flash packaging command,
and the present architecture built around `TriggerPatternPlayer` and its
shared clock.

The obsolete local `BeatClock`, candidate-boundary discussion, and claims that
all modules still awaited extraction are removed. Historical reasoning remains
available in the narrative devlogs rather than competing with current usage
documentation.

## Validation

```text
just package-m5burner
```

The documentation must agree with the compiled dependency graph and preserve a
successful M5Burner package build.
