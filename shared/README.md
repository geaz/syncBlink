[![GPL](https://img.shields.io/github/license/syncBlink/syncBlink)](https://github.com/syncBlink/syncBlink/blob/main/LICENSE)   
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/A0A01MQZP)

# syncBlink Arduino API

This project is meant to be used in combination with a PlatformIO project.
Please have a look at [syncBlink Station](https://github.com/syncBlink/syncBlink/tree/main/station) and [syncBlink Node](https://github.com/syncBlink/syncBlink/tree/main/node) for usage examples.

## Dependencies

Please add the following libraries as dependencies to your PlatformIO project:

- FastLED
- https://github.com/kosme/arduinoFFT.git#35ea7e243f09f6b3e9bee1f6e103ed222ae896de

## Necessary Defines

Please define the following macros:

- LED_PIN       D4
- FFT_SQRT_APPROXIMATION
- FFT_SPEED_OVER_PRECISION

## syncBlink Script

```
let h=0
let s=0
let v=0
let colors = []

let update = fun(delta) {
	if(vol == 0 || freq == 0 || vol < lVol){
        if(v > 0.05){ v = v - 0.05 }
        else{ v = 0 }
    } else {
        h = map(freq, 0, maxF, 180, 0)
        s = 1
        v = map(vol, 0, 100, 0, 1)
    }
    
    for(let i = nLedC - 1; i > 0; i = i - 1) {
        colors[i] = colors[i-1]
    }
    colors[0] = xhsv(h, s, v)
    setLeds(colors)
}

let init = fun(){ setDelay(pLedC) }
let modName = "snakeMod"
```