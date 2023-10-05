//arduino: https://www.reichelt.de/arduino-nano-every-atmega4809-ohne-header-ard-nano-eve-p261300.html?&trstct=pol_0&nbc=1
//led: https://www.reichelt.de/entwicklerboards-neopixel-jewel-ws2812-debo-np-jewel-p235520.html?&trstct=pos_1&nbc=1
//netzteil: https://www.reichelt.de/usb-ladegeraet-5-v-2100-ma-kfz-2-usb-ports-goo-44177-p171361.html?&trstct=pos_5&nbc=1
//gehäuse: https://www.reichelt.de/kleingehaeuse-72-x-50-x-35-mm-geh-ks-35-p8161.html?&trstct=pol_51&nbc=1

#include <algorithm>
#include "Freenove_WS2812_Lib_for_ESP32.h"

#define LEDS_COUNT  24
#define LEDS_PIN  3
#define CHANNEL   0

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL);

#define PIN_TN 4

/*
 * 6000 --> 100.0
 * 6100 --> 101,7
 * 6200 --> 103,3
 * 6300 --> 105
 * 6400 --> 106,7
 * 6500 --> 108,3
 * 6600 --> 110,0
 * 6700 --> 111,7
 * 6800 --> 113,3
 */

#define UPM_6000 100.0
#define UPM_6100 101.7
#define UPM_6200 103.3
#define UPM_6300 105.0
#define UPM_6400 106.7
#define UPM_6500 108.3
#define UPM_6600 110.0
#define UPM_6700 111.7
#define UPM_6800 113.3
#define UPM_6900 115.0

double frequenz[5] = {0.0};
double verarb_frequenz = 0.0;

const int farben[] = {0, 0x00DD00, 0xCCCC00, 0xFF0000};
const double grenzen[] = {0.0, UPM_6100, UPM_6400, UPM_6700};
int farbe = 0;

unsigned long sprung1 = 0, sprung2 = 0;
bool letzter = false;

#include <algorithm>

double getMedian(const double arr[], int size) {
    // Create a copy of the array
    double* sortedArr = new double[size];
    std::copy(arr, arr + size, sortedArr);

    // Sort the copy in ascending order
    std::sort(sortedArr, sortedArr + size);

    // Check if the array size is even or odd
    if (size % 2 == 0) {
        // If the size is even, return the average of the two middle elements
        int midIndex = size / 2;
        double median = (sortedArr[midIndex - 1] + sortedArr[midIndex]) / 2.0;

        // Free the dynamically allocated memory
        delete[] sortedArr;

        return median;
    } else {
        // If the size is odd, return the middle element
        int midIndex = size / 2;
        double median = sortedArr[midIndex];

        // Free the dynamically allocated memory
        delete[] sortedArr;

        return median;
    }
}

void setup() {
  //Serial.begin(9600);

  strip.begin();
  pinMode(PIN_TN, INPUT);

  letzter = false;
  sprung1 = micros();

  for(int i=0; i<LEDS_COUNT; ++i) strip.setLedColorData(i, 0x0);  

  strip.show();

  while(true){
    bool jetzt = digitalRead(PIN_TN);
  
    if ((!letzter && jetzt) || (micros() - sprung1) > 12500) //steigende flanke
    {
      sprung2 = sprung1;
      sprung1 = micros();
  
      unsigned long diff = sprung1 - sprung2;

      frequenz[0] = frequenz[1];
      frequenz[1] = frequenz[2];
      frequenz[2] = frequenz[3];
      frequenz[3] = frequenz[4];
      frequenz[4] = 1000000.0 / (double)diff;

      verarb_frequenz = getMedian(frequenz, sizeof(frequenz)/sizeof(double));
      //Serial.println(verarb_frequenz);

      for (int i = sizeof(grenzen)/sizeof(double)-1; i > -1; --i)
      {
        if (verarb_frequenz >= grenzen[i])
        {
          if (farbe != farben[i])
          {
            for(int j = 0; j < LEDS_COUNT; ++j) {
              strip.setLedColorData(j, farben[i]);
              delay(1);
            }
            strip.show();
            farbe = farben[i];
          }
          break;
        }
      }
    }
    letzter = jetzt;
  }
}

void loop() {}
