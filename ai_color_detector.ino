#define Serial SerialUSB

#include <xCore.h>
#include <xOD01.h>
#include <xSH01.h>
#include <xSL06.h>
#include <SPI.h>
#include <SD.h>


xOD01 OD01;
xSH01 SH01;
xSL06 SL06;

const int cs = 3;

String color_names[14] = {};
uint16_t r[14] = {};
uint16_t g[14] = {};
uint16_t b[14] = {};
uint8_t h[14] = {};
uint8_t s[14] = {};
uint8_t l[14] = {};
bool trained[14] = {};
uint8_t color_index = 0;
bool training = true;
unsigned long diff;
String search_color = "";

struct CHIPS
{
  uint8_t SH01_ACK;
  uint8_t OD01_ACK;
  uint8_t SL06_ACK;
};

struct CHIPS ack;

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println("Initializing xChips...");

  Wire.begin();

  INITIALIZE_XCHIPS();

  Serial.println("Initializing SD card...");

  if (ack.OD01_ACK)
  {
    OD01.clear();
    OD01.home();
    OD01.println("Initializing SD card...");
  }

  if (!SD.begin(cs))
  {
    Serial.println("SD card init. failed");
    if (ack.OD01_ACK)
    {
      OD01.println("SD card init. failed");
    }
    while (true);
  }

  File dataFile = SD.open("colors.txt", FILE_READ);

  Serial.println("Printing color names...");

  if (dataFile)
  {
    String color;
    int count = 0;
    Serial.println("File opened!");
    while (dataFile.available())
    {

      color = dataFile.readStringUntil('\n');
      color_names[count] = color;
      Serial.println(color_names[count]);
      count++;
    }
    dataFile.close();
  } else {
    Serial.println("colors.txt not found");
    if (ack.OD01_ACK)
    {
      OD01.clear();
      OD01.home();
      OD01.println("colors.txt not found");
    }
    while (true);

  }


  if (ack.OD01_ACK)
  {
    OD01.set2X();

    welcome_screen();
  }


  if (ack.OD01_ACK)
  {
    OD01.clear();
    OD01.home();
    OD01.set2X();

    OD01.println(color_names[color_index]);
  }

  for (int i = 0; i < 14; i++)
  {
    r[i] = g[i] = b[i] = h[i] = s[i] = l[i] = 0;
    trained[i] = false;
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  uint16_t red, green, blue;
  uint16_t light;

  uint8_t norm_red, norm_green, norm_blue;

  if (!SL06.getRedLight(red) || !SL06.getGreenLight(green) || !SL06.getBlueLight(blue) || !SL06.getAmbientLight(light)) {
    Serial.println("Error reading light values");
  } else {
    norm_red = map(red, 0, light, 0, 255);
    norm_green = map(green, 0, light, 0, 255);
    norm_blue = map(blue, 0, light, 0, 255);

    Serial.print("RGB : [");
    Serial.print(norm_red);
    Serial.print(", ");
    Serial.print(norm_green);
    Serial.print(", ");
    Serial.print(norm_blue);
    Serial.println("]");
    Serial.print("Light: ");
    Serial.println(light);
  }

  /*********************************************
   *********************************************
     Training the colors label displayed on OD01
   *********************************************
  **********************************************/

  if (training)
  {
    if (SH01.circleTouched())
    {
      while (SH01.circleTouched());

      if (color_index >= 13 )
      {
        color_index = 13;
      } else {
        color_index++;
      }

      if (ack.OD01_ACK)
      {
        OD01.clear();
        OD01.home();
        
        if(trained[color_index])OD01.set1X();
        else OD01.set2X();
        
        OD01.println(color_names[color_index]);
      }
    }

    if (SH01.squareTouched())
    {
      while (SH01.squareTouched());

      if (color_index <= 0)
      {
        color_index = 0;
      } else
      {
        color_index--;
      }

       if(trained[color_index])OD01.set1X();
       else OD01.set2X();

      if (ack.OD01_ACK)
      {
        OD01.clear();
        OD01.home();
        OD01.println(color_names[color_index]);
      }

    }

    if (SH01.triangleTouched())
    {
      while (SH01.triangleTouched());

      uint8_t* hsl2;

      r[color_index] = norm_red;
      g[color_index] = norm_green;
      b[color_index] = norm_blue;

      hsl2 = hsl(norm_red, norm_green, norm_blue);

      h[color_index] = hsl2[0];
      s[color_index] = hsl2[1];
      l[color_index] = hsl2[2];

      trained[color_index] = true;

      OD01.set1X();
      OD01.clear();
      OD01.home();
      OD01.println(color_names[color_index]);

    }

    if (SH01.crossTouched())
    {
      while (SH01.crossTouched());
      if(ack.OD01_ACK)
      {
        OD01.clear();
        OD01.home();
        OD01.set2X();
        OD01.println("TRAINING");
        OD01.println("COMPLETED!");

        delay(3000);

        OD01.clear();
        OD01.home();
        OD01.set2X();
        OD01.println("TESTING");
        OD01.println("STARTS!");
        delay(3000);
      }
      training = false;
    }
  }

  /*********************************************
   *********************************************
    Test different colors
   *********************************************
  **********************************************/

  if (!training)
  {
    calculateColor(norm_red, norm_green, norm_blue);
    Serial.println("Color");

    if(ack.OD01_ACK)
    {
      OD01.clear();
      OD01.home();
      OD01.println(search_color);
      Serial.print(search_color);
      delay(2000);
    }

  }
}

  /*********************************************
   *********************************************
    Functions definitions
   *********************************************
  **********************************************/

void welcome_screen()
{
  OD01.clear();
  OD01.home();
  OD01.println("AI COLOR");
  OD01.println("DETECTOR");

  delay(2000);

  OD01.clear();
  OD01.home();
  OD01.println("TOUCH CRCL");
  OD01.println("/SQR TO");
  OD01.println("CHNG COLR");

  delay(3000);

  OD01.clear();
  OD01.home();
  OD01.println("TOUCH TRI");
  OD01.println("TO TRAIN");

  delay(3000);

  OD01.clear();
  OD01.home();
  OD01.println("TOUCH CROS");
  OD01.println("TO FINISH");
  OD01.println("TRAINING");

  delay(3000);
}

void calculateColor(uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t* hsl2;
  uint8_t hue, sat, light;
  unsigned long ndf1, ndf2, ndf;
  int cl, df;
  int count;

  ndf1 = ndf2 = ndf = 0;
  cl = df = -1;
  hsl2 = hsl(red, green, blue);
  hue = hsl2[0];
  sat = hsl2[1];
  light = hsl2[2];

  count = 0;

  for (int i = 0; i < 14; i++) {

    ndf1 = pow(red - r[i], 2) + pow(green - g[i], 2) + pow(blue - b[i], 2);
    ndf2 = pow(hue - h[i], 2) + pow(sat - s[i], 2) + pow(light - l[i], 2);
    ndf = ndf1 + ndf2 * 2;
    if (df < 0 || df > ndf) {
      df = ndf;
      cl = i;
    }
    count += 1;
  }

  if (count == 14) {
    search_color = color_names[cl];
  }
}

uint8_t* hsl(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t minimum, maximum, delta, hue, sat, light;
  uint8_t hsl_array[3];

  r = r / 255;
  g = g / 255;
  b = b / 255;

  minimum = min(r, min(g, b));
  maximum = max(r, max(g, b));

  delta = maximum - minimum;

  light = (minimum + maximum) / 2;

  sat = 0;
  if (light > 0 && light < 1)
    sat = delta / (light < 0.5 ? (2 * light) : (2 - 2 * light));

  hue = 0;

  if (delta > 0) {
    if (maximum == r && maximum != g) hue += (g - b) / delta;
    if (maximum == g && maximum != b) hue += (2 + (b - r) / delta);
    if (maximum == b && maximum != r) hue += (4 + (r - g) / delta);
    hue /= 6;
  }

  hsl_array[0] = hue * 255;
  hsl_array[1] = sat * 255;
  hsl_array[2] = light * 255;

  return hsl_array;
}


void INITIALIZE_XCHIPS()
{
  if (xCore.ping(0x39))
  {
    ack.SL06_ACK = 0xff;
    SL06.begin();
    SL06.enableLightSensor(false);
  } else {
    ack.SL06_ACK = 0;
  }

  if (xCore.ping(0x3C))
  {
    ack.OD01_ACK = 0xff;
    OD01.begin();
  } else {
    ack.OD01_ACK = 0;
  }


  if (xCore.ping(0x28))
  {
    ack.SH01_ACK = 0xff;
    SH01.begin();
  } else {
    ack.SH01_ACK = 0;
  }
}
