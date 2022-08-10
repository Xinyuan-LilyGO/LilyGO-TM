/*
  Example for TFT_eSPI library

  Created by Bodmer 31/12/16

  This example draws all fonts (as used by the Adafruit_GFX library) onto the
  screen. These fonts are called the GFX Free Fonts (GFXFF) in this library.

  The fonts are referenced by a short name, see the Free_Fonts.h file
  attached to this sketch.

  Other True Type fonts could be converted using the utility within the
  "fontconvert" folder inside the library. This converted has also been
  copied from the Adafruit_GFX library.

  Since these fonts are a recent addition Adafruit do not have a tutorial
  available yet on how to use the fontconvert utility.   Linux users will
  no doubt figure it out!  In the meantime there are 48 font files to use
  in sizes from 9 point to 24 point, and in normal, bold, and italic or
  oblique styles.

  This example sketch uses both the print class and drawString() functions
  to plot text to the screen.

  Make sure LOAD_GFXFF is defined in the User_Setup.h file within the
  library folder.

  --------------------------- NOTE ----------------------------------------
  The free font encoding format does not lend itself easily to plotting
  the background without flicker. For values that changes on screen it is
  better to use Fonts 1- 8 which are encoded specifically for rapid
  drawing with background.
  -------------------------------------------------------------------------

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######       TO SELECT YOUR DISPLAY TYPE AND ENABLE FONTS          ######
  #########################################################################
*/

#include "Free_Fonts.h" // Include the header file attached to this sketch
#include "SPI.h"
#include "SD.h"
#include "TFT_eSPI.h"

#include <btAudio.h>
#include<WebServer.h>

// Sets the name of the audio device
btAudio audio = btAudio("LilyGoSpeaker");
// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();


#define IIS_BCK   26
#define IIS_WS    25
#define IIS_DOUT  19

#define SDCARD_MISO   2
#define SDCARD_MOSI   15
#define SDCARD_SCLK   14
#define SDCARD_CS     13

SPIClass SDSPI = SPIClass(HSPI);



void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void setup(void)
{
    Serial.begin(115200);

    tft.begin();

    tft.setRotation(1);
    // Set text datum to middle centre
    tft.setTextDatum(MC_DATUM);

    // Set text colour to orange with black background
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);            // Clear screen
    tft.setFreeFont(FF18);                 // Select the font
    tft.drawString("LilyGo TM", 160, 60, GFXFF);// Print the string name of the font
    tft.setFreeFont(FF1);                 // Select the font
    tft.drawString("SDCard BT Play demo", 160, 120, GFXFF);// Print the string name of the font

    // Streams audio data to the ESP32
    audio.begin();

    // Re-connects to last connected device
    audio.reconnect();

    // Outputs the received data to an I2S DAC, e.g. https://www.adafruit.com/product/3678
    audio.I2S(IIS_BCK, IIS_DOUT, IIS_WS);

    pinMode(SDCARD_MISO, INPUT_PULLUP);
    SDSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);

    if (!SD.begin(SDCARD_CS, SDSPI)) {
        Serial.println("Begin SDCARD FAILED!!!");
        tft.drawString("SDCard not found!", 160, 140, GFXFF);
        while (true);
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
}

void loop()
{
    delay(1000);
}

