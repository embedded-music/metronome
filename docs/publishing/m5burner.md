# Publishing Metronome with M5Burner

## Build the image

Set the release version in `VERSION`, then run:

```text
just package-m5burner
```

The command builds the firmware and copies PlatformIO's merged factory image
to:

```text
dist/m5burner/metronome-m5stickc-plus2-v0.1.0.bin
```

A SHA-256 checksum is written alongside it. The generated artifacts are
ignored by Git and should be attached to a matching tagged GitHub release.

The merged image is flashed at address `0x0`; it contains the bootloader,
partition table, Arduino boot metadata, and application. Do not submit the
smaller `firmware.bin`, which contains only the application image.

## Hardware release gate

1. Flash the generated merged image at address `0x0` on an M5StickC Plus2.
2. Confirm the display boots in landscape and the opening accented click plays.
3. Let it run long enough to confirm a stable four-beat visual and audible cycle.
4. Check the minimum and maximum tempo and volume boundaries.
5. Short-click Power to cycle through Tempo, Volume, and Sound; confirm the
   on-screen instruction and audition both sound roles with A and B.
6. Burn the same image through M5Burner's local/custom firmware flow.

Command-line flashing equivalent:

```text
esptool --chip esp32 write-flash 0x0 \
  dist/m5burner/metronome-m5stickc-plus2-v0.1.0.bin
```

## M5Burner metadata

| Field | Value |
| --- | --- |
| Name | `Metronome` |
| Version | value from `VERSION` |
| Device Type | M5StickC Plus2 |
| GitHub | `https://github.com/embedded-music/metronome` |
| Firmware | generated versioned `.bin` |
| Description | `Pocket metronome for M5StickC Plus2 with visual beats, an accented downbeat, adjustable tempo and volume, and selectable click sounds.` |

Sign in to M5Burner, open **USER CUSTOM** and **Publish**, then upload the
firmware and a cover image. Publish privately first, burn the uploaded copy on
the target device, and only then make the entry public.

## Updating a release

Change `VERSION`, rebuild, repeat the hardware gate, and upload the new version
through the existing entry's **Detail** action. Keep the source revision
discoverable through a matching Git tag and GitHub release.

Official references:

- [M5Stack: Publish Firmware](https://docs.m5stack.com/en/uiflow/m5burner/publish)
- [M5Stack: Add custom firmware](https://docs.m5stack.com/en/related_documents/M5Burner)
