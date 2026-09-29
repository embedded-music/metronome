# 016 — Name the next Power-button screen

## Goal

Make the persistent Power-button hint tell the player what will happen next.

## Change

The footer now derives its Power label from the current control mode:

| Current screen | Footer destination |
| --- | --- |
| BPM | `PWR Volume` |
| Volume | `PWR Sounds` |
| Sounds | `PWR BPM` |

The A/B assignments remain on the same row. This replaces the generic
`PWR screen` wording without changing the three-screen cycle.

## Validation

```text
just package-m5burner
```

The firmware must build and regenerate the merged image. Hardware validation
should confirm every label fits the display and matches the screen reached by
the next short Power click.
