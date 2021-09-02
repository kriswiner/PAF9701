# PAF9701
Arduino sketches for PixArt Imaging's 8 x 8 pixel IR thermal imaging camera

   PixArt Imaging's 8 x 8 pixel IR thermal imaging sensor
   60 deg field of view, 2 mA normal mode current, -20 to 380 C object
   temperature range, 16-bit object temperatures to +/- 1 degree accuracy or better.

   The sketch demonstrates how to cinitialize the PAF9701 in normal run mode, configure the
   data filters and image orientation, set up the data ready interrupt, read the data and plot 
   the properly scaled data on the serial monitor and on a 160 x 128 pixel Adafruit  TFT color display.

   The sketch is intended to run using a Tlera Corporation STM32L432 Ladybug development board but just about
   and 3V3 dev board with an SPI port (for the display) and I2C port (for the PAF9701) will do.

   This sketch may be used without limitations with proper attribution
