# 017 — Put A and B before Power in the footer

## Goal

Make the footer read in the same order as the controls used most often.

## Change

Every screen now lists the current A and B actions first and the Power-button
destination last:

```text
A +   B -   PWR Volume
A +   B -   PWR Sounds
A accent  B regular  PWR BPM
```

The interaction and screen cycle are unchanged.

## Validation

```text
just package-m5burner
```

The firmware must build and regenerate the merged image. Hardware validation
should confirm that the longest Sounds footer fits on the display.
