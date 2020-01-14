 /* button-based-audio-player
  || @version 1.5
  || @author Gerry Straathof
  || @contact nosemonger@gmail.com
  ||
  || @description
  || | read a button, play a file.
  || #
*/
#include <Keypad.h>

//this is to identify and change the behavior of pins on the featherwing to 
//allow them to be used as GPIO pins for the keyboard matrix
// since the tpyical digital pins are used by the featherwing

#define A5_INTERRUPT_PIN 19  // A5's interrupt is on raw channel 19
#define A4_INTERRUPT_PIN 18  // A4's interrupt is on raw channel 18
#define A3_INTERRUPT_PIN 17  // A3's interrupt is on raw channel 17
#define A2_INTERRUPT_PIN 16  // A2's interrupt is on raw channel 16
#define A1_INTERRUPT_PIN 15  // A1's interrupt is on raw channel 15


const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {     //these numbers are coded for the unit
  {'4', '6', '5'},            //do not change these or you risk
  {'7', '9', '8'},            //reorganizing the buttons out of order
  {'1', '3', '2'},
  {'A', 'C', 'B'}
};

                              //these pins are hard wired to the audio featherwing
byte rowPins[ROWS] = {11 , 13, 15, 16}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {17, 18, 19};      //connect to the column pinouts of the keypad

                              //this makes the buttons behave as a keypad
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


/// audio related stuff
// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)

                              //while unnecessary I have left these in 
                              //in case others want to do similar things
// Feather ESP8266
#if defined(ESP8266)
#define VS1053_CS      16     // VS1053 chip select pin (output)
#define VS1053_DCS     15     // VS1053 Data/command select pin (output)
#define CARDCS          2     // Card chip select pin
#define VS1053_DREQ     0     // VS1053 Data request, ideally an Interrupt pin

// Feather ESP32
#elif defined(ESP32)
#define VS1053_CS      32     // VS1053 chip select pin (output)
#define VS1053_DCS     33     // VS1053 Data/command select pin (output)
#define CARDCS         14     // Card chip select pin
#define VS1053_DREQ    15     // VS1053 Data request, ideally an Interrupt pin

// Feather Teensy3
#elif defined(TEENSYDUINO)
#define VS1053_CS       3     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          8     // Card chip select pin
#define VS1053_DREQ     4     // VS1053 Data request, ideally an Interrupt pin

// WICED feather
#elif defined(ARDUINO_STM32_FEATHER)
#define VS1053_CS       PC7     // VS1053 chip select pin (output)
#define VS1053_DCS      PB4     // VS1053 Data/command select pin (output)
#define CARDCS          PC5     // Card chip select pin
#define VS1053_DREQ     PA15    // VS1053 Data request, ideally an Interrupt pin

#elif defined(ARDUINO_NRF52832_FEATHER )
#define VS1053_CS       30     // VS1053 chip select pin (output)
#define VS1053_DCS      11     // VS1053 Data/command select pin (output)
#define CARDCS          27     // Card chip select pin
#define VS1053_DREQ     31     // VS1053 Data request, ideally an Interrupt pin

                               //// this is the only bit neccessary for 
                               //// feather M0 board and musicmaker
// Feather M4, M0, 328, nRF52840 or 32u4
#else
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
// DREQ should be an Int pin *if possible* (not possible on 32u4)
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

#endif


Adafruit_VS1053_FilePlayer musicPlayer =
      Adafruit_VS1053_FilePlayer(VS1053_RESET, 
                                 VS1053_CS, 
                                 VS1053_DCS, 
                                 VS1053_DREQ, 
                                 CARDCS);

 
void setup() {
  Serial.begin(9600);

/*
  // Wait for serial port to be opened, remove this line for 'standalone' operation
  while (!Serial) {
    delay(1);
  }
*/
  delay(1000);
  Serial.println("\n\nAdafruit VS1053 Feather Test");

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }

  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");

  // list files
  Serial.println("these are the files on the SD card:");
  printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(5, 5);
  
  #if defined(__AVR_ATmega32U4__)
    // Timer interrupts are not suggested, better to use DREQ interrupt!
    // but we don't have them on the 32u4 feather...
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int
  #else
    // If DREQ is on an interrupt pin we can do background
    // audio playing
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  #endif

  //uncomment this line if you would like to have a tone play
  //musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working

  // Play a file in the background, REQUIRES interrupts!
  //this file will not be accessible to the button array.
  Serial.println(F("Playing boot audio"));
  musicPlayer.playFullFile("track013.mp3");
  
} 

/*
// the main loop is basically get a key if pressed, and play an audio file for each key.
*/

void loop() {
  char customKey = customKeypad.getKey();

  
  if (customKey) {
    Serial.println(customKey);
    delay(10);
    
    if (customKey == '1') {
      Serial.println(F("Playing full track 001"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track001.mp3");
    }

    if (customKey == '2') {
      Serial.println(F("Playing full track 002"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track002.mp3");
    }
    
    if (customKey == '3') {
      Serial.println(F("Playing full track 003"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track003.mp3");
    }
    if (customKey == '4') {
      Serial.println(F("Playing full track 004"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track004.mp3");
    }
    if (customKey == '5') {
      Serial.println(F("Playing full track 005"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track005.mp3");
    }
    if (customKey == '6') {
      Serial.println(F("Playing full track 006"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track006.mp3");
    }
    if (customKey == '7') {
      Serial.println(F("Playing full track 007"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track007.mp3");
    }
    if (customKey == '8') {
      Serial.println(F("Playing full track 008"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track008.mp3");
    }
    if (customKey == '9') {
      Serial.println(F("Playing full track 009"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track009.mp3");
    }
    if (customKey == 'A') {
      Serial.println(F("Playing full track 010"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track010.mp3");
    }
    if (customKey == 'B') {
      Serial.println(F("Playing full track 011"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track011.mp3");
    }
    if (customKey == 'C') {
      Serial.println(F("Playing full track 012"));
      musicPlayer.pausePlaying(true);
      musicPlayer.startPlayingFile("/track012.mp3");
    }
  }
}


/// File listing helper for troubleshouting
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
