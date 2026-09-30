# 018 — Describe the Plus2 output as a passive buzzer

## Goal

Set accurate expectations for the M5StickC Plus2 audio hardware.

## Change

User-facing documentation now calls the built-in transducer an onboard passive
buzzer, matching M5Stack's hardware specification. `Speaker_Class` remains an
implementation detail of M5Unified and is not presented as evidence that the
device contains a conventional speaker.

The README architecture summary and proposed M5Burner description use the same
terminology.

## Validation

```text
just package-m5burner
```

The documentation-only change must leave the release artifact reproducible.
