#include "SPIFFS.h"

static const char * TAG ="MLX90640_IMAGE";
void ThermalImageToWeb(float mlx90640To[],const uint16_t camColors[], float MinTemp, float MaxTemp)
{
  // --- SAVE BMP FILE --- //
  uint8_t colorIndex = 0;
  uint16_t color = 0;
  unsigned int headers[13];
  int extrabytes;
  int paddedsize;
  int x = 0; 
  int y = 0; 
  int n = 0;
  int red = 0;
  int green = 0;
  int blue = 0;
  
  int WIDTH = 32;
  int HEIGHT = 24;

  extrabytes = 4 - ((WIDTH * 3) % 4);                 // How many bytes of padding to add to each
                                                    // horizontal line - the size of which must
                                                    // be a multiple of 4 bytes.
  if (extrabytes == 4)
    extrabytes = 0;

  paddedsize = ((WIDTH * 3) + extrabytes) * HEIGHT;

// Headers...
// Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".

  headers[0]  = paddedsize + 54;      // bfSize (whole file size)
  headers[1]  = 0;                    // bfReserved (both)
  headers[2]  = 54;                   // bfOffbits
  headers[3]  = 40;                   // biSize
  headers[4]  = WIDTH;                // biWidth
  headers[5]  = HEIGHT;               // biHeight

// Would have biPlanes and biBitCount in position 6, but they're shorts.
// It's easier to write them out separately (see below) than pretend
// they're a single int, especially with endian issues...

  headers[7]  = 0;                    // biCompression
  headers[8]  = paddedsize;           // biSizeImage
  headers[9]  = 0;                    // biXPelsPerMeter
  headers[10] = 0;                    // biYPelsPerMeter
  headers[11] = 0;                    // biClrUsed
  headers[12] = 0;                    // biClrImportant

// outfile = fopen(filename, "wb");

  File file =  SPIFFS.open("/thermal.bmp", "wb");
  if (!file) {
   ESP_LOGI(TAG,"There was an error opening the file for writing");
    //return;
  }else{

// Headers begin...
// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.

  file.print("BM");

  for (n = 0; n <= 5; n++)
  { 
    file.printf("%c", headers[n] & 0x000000FF);
    file.printf("%c", (headers[n] & 0x0000FF00) >> 8);
    file.printf("%c", (headers[n] & 0x00FF0000) >> 16);
    file.printf("%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
  }

// These next 4 characters are for the biPlanes and biBitCount fields.

  file.printf("%c", 1);
  file.printf("%c", 0);
  file.printf("%c", 24);
  file.printf("%c", 0);

  for (n = 7; n <= 12; n++)
  {
    file.printf("%c", headers[n] & 0x000000FF);
    file.printf("%c", (headers[n] & 0x0000FF00) >> 8);
    file.printf("%c", (headers[n] & 0x00FF0000) >> 16);
    file.printf("%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
  }

// Headers done, now write the data...

  for (y = HEIGHT - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
  {
    for (x = 0; x <= WIDTH - 1; x++)
    {
      // --- Read ColorIndex corresponding to Pixel Temperature --- //
      colorIndex = map(mlx90640To[x+(32*y)], MinTemp-5.0, MaxTemp+5.0, 0, 255);
      colorIndex = constrain(colorIndex, 0, 255);
      color = camColors[colorIndex];
      
      // --- Converts 4 Digits HEX to RGB565 --- //
      // uint8_t r = ((color >> 11) & 0x1F);
      // uint8_t g = ((color >> 5) & 0x3F);
      // uint8_t b = (color & 0x1F);

      // --- Converts 4 Digits HEX to RGB565 -> RGB888 --- //
      red = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
      green = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
      blue = (((color & 0x1F) * 527) + 23) >> 6;

      // --- RGB range from 0 to 255 --- //
      if (red > 255) red = 255; if (red < 0) red = 0;
      if (green > 255) green = 255; if (green < 0) green = 0;
      if (blue > 255) blue = 255; if (blue < 0) blue = 0;

      // Also, it's written in (b,g,r) format...

      file.printf("%c", blue);
      file.printf("%c", green);
      file.printf("%c", red);
    }
    if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
    {
      for (n = 1; n <= extrabytes; n++)
      {
         file.printf("%c", 0);
      }
    }
  }

  file.close();
  ESP_LOGI(TAG,"Updated and Closed");
  }         // --- END SAVING BMP FILE --- //
}
