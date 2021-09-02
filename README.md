# PAF9701
Arduino sketches for PixArt Imaging's 8 x 8 pixel IR thermal imaging camera

![image](https://user-images.githubusercontent.com/6698410/131921078-3c8f8538-b9d3-4090-8422-59423026ff9d.jpg)

*Adafruit 160 x 128 pixel TFT color display showing thermal image of my hand above the sensor.*

PixArt Imaging's 8 x 8 pixel IR thermal imaging sensor 60 deg field of view, 2 mA normal mode current, -20 to 380 C object temperature range, 16-bit object temperatures to +/- 1 degree accuracy or better.

The sketch demonstrates how to cinitialize the PAF9701 in normal run mode, configure the data filters and image orientation, set up the data ready interrupt, read the data and plot the properly scaled data on the serial monitor and on a 160 x 128 pixel Adafruit  TFT color display.

The sketch is intended to run using a Tlera Corporation STM32L432 [Ladybug](https://www.tindie.com/products/tleracorp/ladybug-stm32l432-development-board/) development board but just about and 3V3 dev board with an SPI port (for the display) and I2C port (for the PAF9701) will do.

This sketch may be used without limitations with proper attribution
