[![GPL](https://img.shields.io/github/license/syncBlink/syncBlink)](https://github.com/syncBlink/syncBlink/blob/main/LICENSE)   
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A0A01MQZP)

# syncBlink Arduino API

This project is meant to be used in combination with a PlatformIO project.
Please have a look at [syncBlink Station](https://github.com/syncBlink/syncBlink/tree/main/station) and [syncBlink Node](https://github.com/syncBlink/syncBlink/tree/main/node) for usage examples.

## Dependencies

Please add the following libraries as dependencies to your PlatformIO project:

- FastLED
- https://github.com/kosme/arduinoFFT.git#35ea7e243f09f6b3e9bee1f6e103ed222ae896de

## Necessary PlatformIO build flags

Define the following build flags

build_flags =
    -DVERSIONMAJOR=1
    -DVERSIONMINOR=0
    -DLED_PIN=D4
    -DFFT_SQRT_APPROXIMATION
    -DFFT_SPEED_OVER_PRECISION
    -DLWIP_INTERNAL