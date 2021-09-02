# PAF9701
Arduino sketches for PixArt Imaging's 8 x 8 pixel IR thermal imaging camera

![image](https://user-images.githubusercontent.com/6698410/131921078-3c8f8538-b9d3-4090-8422-59423026ff9d.jpg)

*Adafruit 160 x 128 pixel TFT color display showing thermal image of my hand above the PAF9701 sensor breakout board.*

PixArt Imaging's 8 x 8 pixel IR thermal imaging sensor offers wide (60 deg) field of view, low-power (2 mA) normal mode current usage, wide (-20 to 380 C) object temperature range, and accurate (+/- 1 degree C) 16-bit object temperatures.

The NormalMode sketch demonstrates how to initialize the PAF9701 image sensor in normal run mode, configure the data filters and image orientation, set up the data ready interrupt, read the data and plot the properly scaled data on the serial monitor and on a 160 x 128 pixel Adafruit TFT color display.

I will be adding sketches demonstrating auto low power modes and selecting different pixel patterns as well as interrupting on absolute and differential thresholds, etc. This sensor can do quite a lot; more than can be reasonably demonstrated in one simple sketch.

The sketches are intended to run using a Tlera Corporation STM32L432 [Ladybug](https://www.tindie.com/products/tleracorp/ladybug-stm32l432-development-board/) development board but just about and 3V3 dev board with an SPI port (for the display) and I2C port (for the PAF9701) will do.

These sketches may be used without limitations with proper attribution

PAF9701 breakout [design](https://oshpark.com/shared_projects/jREzx9Yg) may be obtained in the shared space of OSHPark. Ask PixArtImaging for information on sampling the PAF9701 thermal imaging sensor.
