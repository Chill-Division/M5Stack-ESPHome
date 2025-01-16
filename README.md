# M5Stack-ESPHome
Collection of information for getting M5Stack sensors / controllers working with ESPHome

If you're on mobile, tap "view code" above to see the files containing example yaml for ESPHome based on the name on the M5Stack webaite

These vary between the PoESP32 and the Atom Lite. Take note before copy / pasting:
| Color  | Atom Lite / Echo | Atom Lite S3 / Nano C6 | PoESP32 | PoECam / PoECam-W | VoicePE |
| ------------- | ------------- | ------------- | ------------- | ------------- | ------------- |
| Yellow  | sda: 26  |  sda: 2  | sda: 16  | sda: 25 | sda: 1 |
| White  | scl: 32  | scl: 1  | scl: 17 | scl: 33 | scl: 2 |

M5StampS3 Breakout board:
| Color | J1 - GPIO | J2 - GPIO | J3 - X | J4 - SDA/SCL | J5 - GPIO | J6 - GPIO |
| ------------- | ------------- | ------------- | ------------- | ------------- | ------------ | ------------- |
| Yellow  | G2 | G3 | G13 | G7 | G5 | G11 |
| White  | G1 | G4 | G15 | G9 | G6 | G10 |
