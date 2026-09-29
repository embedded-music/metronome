# 015 — Change screens with the Power button

## Goal

Make the M5StickC Plus2 controls follow the device's physical hierarchy and
explain the interaction directly on screen.

## Interaction

A short Power-button click now advances through Tempo, Volume, and Sound. The
old A+B chord is removed, leaving A and B dedicated to the two actions shown on
the current screen. M5Unified's `BtnPWR.wasClicked()` distinguishes the short
UI gesture from the device's long-hold power behavior.

The persistent bottom row reads `PWR screen   A +   B -` on numeric screens
and `PWR screen  A accent  B regular` on the sound screen. The instruction is
therefore visible without consulting the README.

## Validation

```text
just package-m5burner
```

The firmware must compile and produce the versioned merged image. Hardware
validation should short-click Power repeatedly and confirm the three-screen
cycle, verify that A+B no longer changes screens, and confirm a long Power hold
still follows the device's normal power behavior.
