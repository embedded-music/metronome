# 014 — Graduate the metronome to an M5Burner release candidate

## Goal

Turn the working M5StickC Plus2 experiment into a reproducible, independently
buildable firmware artifact suitable for M5Burner's community directory.

## Release boundary

The application now consumes `fcz2/m5-tone-output@0.1.5` and
`fcz2/step-trigger@0.2.0` from the PlatformIO Registry. A clean checkout no
longer depends on sibling repositories, matching the release-soon workflow
used by the electronic drummer showcase.

`VERSION` is the single firmware version input. `just package-m5burner` builds
the M5StickC Plus2 environment, copies PlatformIO's full factory image to a
versioned name under `dist/m5burner/`, and writes its SHA-256 checksum. The
application-only image is deliberately not packaged.

The README now presents the repository as a usable firmware rather than an
unfinished clock experiment. The publishing guide records the hardware gate,
M5Burner flow, and initial directory metadata.

## Validation

```text
just package-m5burner
```

The command must resolve only published application dependencies, build the
firmware, and produce the versioned merged image plus checksum. Flashing that
exact image and completing the hardware gate remain required before public
submission.
