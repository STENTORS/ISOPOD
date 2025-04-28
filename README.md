# 🎵 Isopod Music Player

An Arduino-based music player using **DFPlayer Mini**, an **OLED display**, and **three buttons** for track control, volume adjustment, and page switching.

## Features

- Play/Pause music with a button.
- Next/Previous track and volume control.
- Double-click to switch between Home and Isopod (Game) page.
- OLED display shows track info, volume, and animations.
- Game mode - Tamagotchi insidered game with the Isopod 
## Hardware

- Arduino Nano
- DFPlayer Mini + MicroSD card
- 1.3" OLED 
- 3 Push Buttons
- Speaker

## Libraries

- [DFRobotDFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini)
- [SoftwareSerial](https://www.arduino.cc/en/Reference/SoftwareSerial)
- [U8g2](https://github.com/olikraus/u8g2)

## Pinout

| Function                  | Pin  |
|---------------------------|------|
| DFPlayer RX/TX            | 10/9 |
| Buttons (Next/Pause/Prev) | 2/3/4|
| OLED                      | A4/A5|

## Controls

- **Pause button**:  
  - Single click → Play/Pause  
  - Double click → Switch page
- **Next/Prev buttons**:  
  - Short press → Next/Previous track  
  - Hold → Volume up/down

## Notes
- MP3s must be named `0001.mp3`, `0002.mp3`, etc.

## License
This project is licensed under the MIT License.
