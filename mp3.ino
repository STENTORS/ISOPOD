#include <DFRobotDFPlayerMini.h> 
#include <SoftwareSerial.h>
#include <U8g2lib.h>

// OLED Display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// DFPlayer
SoftwareSerial mySerial(10, 9);
DFRobotDFPlayerMini myDFPlayer;

// Buttons
#define buttonNext 2
#define buttonPause 3
#define buttonPrevious 4
#define ACTIVATED LOW

// State
int currentTrack = 1;
bool isPlaying = true;
int currentVolume = 20;
int currentPage = 0; // 0 = Home, 1 = Game

// Double-click handling
bool pauseClicked = false;
unsigned long lastPauseClickTime = 0;
const unsigned long doubleClickThreshold = 250; // milliseconds
bool waitingForSecondClick = false;

void setup() {
  u8g2.begin();

  pinMode(buttonPause, INPUT_PULLUP);
  pinMode(buttonNext, INPUT_PULLUP);
  pinMode(buttonPrevious, INPUT_PULLUP);

  mySerial.begin(9600);
  delay(1000);

  if (!myDFPlayer.begin(mySerial)) {
    drawError("DFPlayer Error");
    while (1);
  }

  myDFPlayer.volume(currentVolume);
  myDFPlayer.play(currentTrack);

  flashPage();
  delay(2000);
  updateDisplay();
}

void loop() {
  static bool updateNeeded = false;

  // ---- Pause Button (Single click or Double-click) ----
  if (digitalRead(buttonPause) == ACTIVATED) {
    unsigned long now = millis();

    if (waitingForSecondClick && (now - lastPauseClickTime < doubleClickThreshold)) {
      // Double-click detected
      currentPage = (currentPage == 0) ? 1 : 0;
      waitingForSecondClick = false;
      pauseClicked = false;
      updateNeeded = true;
      while (digitalRead(buttonPause) == ACTIVATED); // wait release
      delay(50); // debounce
    } else if (!pauseClicked) {
      // First click detected
      pauseClicked = true;
      lastPauseClickTime = now;
      waitingForSecondClick = true;
      while (digitalRead(buttonPause) == ACTIVATED); // wait release
      delay(50); // debounce
    }
  }

  // After waiting, if no second click: treat as single click
  if (waitingForSecondClick && (millis() - lastPauseClickTime > doubleClickThreshold)) {
    // Single click confirmed (no second click)
    if (pauseClicked) {
      if (isPlaying) {
        myDFPlayer.pause();
        isPlaying = false;
      } else {
        myDFPlayer.start();
        isPlaying = true;
      }
      pauseClicked = false;
      waitingForSecondClick = false;
      updateNeeded = true;
    }
  }

  // ---- Next Button ----
  static unsigned long buttonNextPressedTime = 0;
  if (digitalRead(buttonNext) == ACTIVATED) {
    if (buttonNextPressedTime == 0) {
      buttonNextPressedTime = millis();
    } else if (millis() - buttonNextPressedTime > 800) {
      if (currentVolume < 30) currentVolume++;
      myDFPlayer.volume(currentVolume);
      updateNeeded = true;
      delay(300);
    }
  } else if (buttonNextPressedTime != 0) {
    if (millis() - buttonNextPressedTime < 800) {
      currentTrack++;
      myDFPlayer.play(currentTrack);
      isPlaying = true;
    }
    buttonNextPressedTime = 0;
    updateNeeded = true;
  }

  // ---- Previous Button ----
  static unsigned long buttonPreviousPressedTime = 0;
  if (digitalRead(buttonPrevious) == ACTIVATED) {
    if (buttonPreviousPressedTime == 0) {
      buttonPreviousPressedTime = millis();
    } else if (millis() - buttonPreviousPressedTime > 800) {
      if (currentVolume > 0) currentVolume--;
      myDFPlayer.volume(currentVolume);
      updateNeeded = true;
      delay(300);
    }
  } else if (buttonPreviousPressedTime != 0) {
    if (millis() - buttonPreviousPressedTime < 800) {
      if (currentTrack > 1) currentTrack--;
      myDFPlayer.play(currentTrack);
      isPlaying = true;
    }
    buttonPreviousPressedTime = 0;
    updateNeeded = true;
  }

  // ---- Refresh display if needed ----
  if (updateNeeded) {
    updateDisplay();
    updateNeeded = false;
  }
}

// ----------------- UI Functions -----------------

// Bitmap for Isopod
const unsigned char epd_bitmap_isopod [] PROGMEM = {
  0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x0c,0x00,
  0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,
  0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,
  0x00,0x30,0x00,0x00,0xfc,0x0f,0x30,0x00,0x00,0xfc,0x0f,0x30,0x00,0x00,0x03,0x3c,
  0x3c,0x00,0x00,0x03,0x3c,0x3c,0x00,0x00,0xcf,0xf3,0x0c,0x00,0x00,0xcf,0xf3,0x0c,
  0x00,0xf0,0xf3,0xcf,0xff,0x00,0xf0,0xf3,0xcf,0xff,0x00,0xf3,0xfc,0xf3,0x3f,0x00,
  0xf3,0xfc,0xf3,0x3f,0xf0,0xfc,0xfc,0x0c,0xcf,0xf0,0xfc,0xfc,0x0c,0xcf,0x3c,0xff,
  0xfc,0xf0,0x3c,0x3c,0xff,0xfc,0xf0,0x3c,0x3f,0x0f,0x33,0x0f,0x03,0x3f,0x0f,0x33,
  0x0f,0x03,0x00,0x30,0xcf,0x33,0x0f,0x00,0x30,0xcf,0x33,0x0f
};





const uint8_t bitmap_width = 40;
const uint8_t bitmap_height = 28;


//start page
void flashPage() {
  u8g2.clearBuffer();
  u8g2.drawFrame(10, 10, 108, 44);
  u8g2.setFont(u8g2_font_pcsenior_8u);
  u8g2.drawXBMP(60, 25, bitmap_width, bitmap_height, epd_bitmap_isopod);
  u8g2.setCursor(30, 35);
  u8g2.print(F("ISOPOD"));
  u8g2.sendBuffer();
}


//header
void drawHeader() {
  u8g2.setDrawColor(1);
  u8g2.drawFrame(0, 0, 128, 14);

  u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
  if (currentPage == 0) {
    u8g2.drawGlyph(5, 11, 0x0040); // Home icon
  } else {
    u8g2.setFont(u8g2_font_unifont_t_animals);
    u8g2.drawGlyph(5, 11, 0x0230); // Animal icon
  }

  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.setCursor(90, 11);
  u8g2.print("VOL ");
  u8g2.print(currentVolume);

  u8g2.setDrawColor(1);
}


//draw the page to screen
void updateDisplay() {
  u8g2.clearBuffer();
  drawHeader();

  if (currentPage == 0) {
    drawHomePage();
  } else {
    drawGamePage();
  }

  u8g2.sendBuffer();
}


//home page
void drawHomePage() {
  u8g2.drawFrame(0, 15, 128, 33);

  u8g2.setFont(u8g2_font_pcsenior_8u);
  u8g2.setCursor(10, 35);
  u8g2.print("SONG:");
  u8g2.setCursor(60, 35);
  u8g2.print(currentTrack);

  u8g2.drawFrame(0, 50, 128, 14);

  u8g2.setFont(u8g2_font_open_iconic_play_1x_t);
  u8g2.drawGlyph(25, 61, 0x0047); // Previous
  u8g2.drawGlyph(60, 61, isPlaying ? 0x0044 : 0x0045); // Play/Pause
  u8g2.drawGlyph(95, 61, 0x0048); // Next
}


//isopod page
void drawGamePage() {
  u8g2.drawFrame(0, 15, 128, 49);
  
  u8g2.drawXBMP(44, 25, bitmap_width, bitmap_height, epd_bitmap_isopod);
}

void drawError(const char* msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 30, msg);
  u8g2.sendBuffer();
}
