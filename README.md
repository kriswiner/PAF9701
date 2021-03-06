# PAF9701
Arduino sketches for PixArt Imaging's 8 x 8 pixel low-power IR thermal imaging camera

![image](https://user-images.githubusercontent.com/6698410/131921078-3c8f8538-b9d3-4090-8422-59423026ff9d.jpg)

*Adafruit 160 x 128 pixel TFT color display showing thermal image of my hand above the PAF9701 sensor breakout board.*

PixArt Imaging's 8 x 8 pixel low-power IR thermal imaging sensor offers wide (60 deg) field of view, low-power (2 mA) normal mode current usage, wide (-20 to 380 C) object temperature range, and accurate (+/- 1 degree C) 16-bit object temperatures.

The **NormalMode** sketch demonstrates how to initialize the PAF9701 image sensor in normal run mode, configure the data filters and image orientation, set up the data ready interrupt, read the data and plot the properly scaled data on the serial monitor and on a 160 x 128 pixel Adafruit TFT color display.

The **autoPowerSaveMode** sketch demonstrates how to take advantage of the built-in power management mode. In autoPowerSaveMode, the sensor starts out in normal run mode. If one sets a data ready alert, the sensor simply behaves as it does in the normal run mode sketch and updates the display at the requested sample rate (I typically use 4 Hz). If the user instead specifies a temperature threshold limit (either absolute as in this sketch or differential) the normal run mode checks for an alert condition at the sample rate but only updates the display when the alert condition obtains, i.e., when there is an object in the field of view with enough pixels above/below the programmed thresholds. 

Normal run mode operation (2 mA) transitions to detectMode1 (310 uA) after a user specified delay time (60 seconds in the sketch) without an alert. In detectMode1, the sensor checks for an alert condition every 20 seconds. If there is no alert condition observed within the delay time in DetectMode 1 the sensor drops down into detectMode2 (52 uA), where the sensor checks for an alert every 120 seconds. Once an alert condition is detected the sensor returns to normal run mode and the process starts over again.

So the power usage drops by factors of ~6-7 at each stage and the latency increases by about the same amount. This provides a way for the user to manage power usage that is very convenient and effective, and offers enough flexibility that the power usage and latency can be tailored to the specific application without elaborate host programming.

The **GestureDetection** sketch demonstrates how to initialize the PAF9701 in normal run mode, configure the temperature limit thresholds and hystereses, configure and report the alert flags, read the data and plot the properly scaled data on the serial monitor and on a 160 x 128 pixel Adafruit TFT color display.  The sketch keeps track of the pixels that exceed the temperature threshold conditions specified by the user, calculates the centroid of the pixels with 1 pixel resolution, and then compares successive centroids to recognize hand gestures like swipe left, swipe up, etc. This could be useful, for example, for touchless control applications.

This fairly primitive capability could easily be extended to detect and count people and/or animal transits across the field of view, track movements into or out of a space, etc. It could also be used to classify and track more sophisticated individual limb and hand motions.

I will be adding sketches as new applications are developed. This sensor can do quite a lot; more than can be reasonably demonstrated in one simple sketch.

The sketches are intended to run using a Tlera Corporation STM32L432 [Ladybug](https://www.tindie.com/products/tleracorp/ladybug-stm32l432-development-board/) development board but just about any 3V3 dev board with an SPI port (for the display) and I2C port (for the PAF9701) will do.

These sketches may be used without limitations with proper attribution.

The 18 mm x 10 mm breadboard-compatible PAF9701 breakout [design](https://oshpark.com/shared_projects/jREzx9Yg) may be obtained in the shared space of OSHPark. Ask PixArt Imaging for information on sampling the PAF9701 thermal imaging sensor.
