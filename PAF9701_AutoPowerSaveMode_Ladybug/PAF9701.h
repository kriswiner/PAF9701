/* September 6, 2021 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 *  Simple library for configuring and reading data from PixArt Imaging's PAF9701
 *  8 x 8 pixel low-power thermal (IR) imaging camera.
 *  
 *  Taking advantage of the auto power save mode, configuring the temperature limit windows, reporting
 *  the alert flags and alert pixels.
 *  
 *  
 *  Library may be used freely and without limit with attribution.
 *  
 */
 
#ifndef PAF9701_h
#define PAF9701_h

#include "Arduino.h"
#include "I2CDev.h"
#include <Wire.h>

/* Register Map PAF9701
 * There are five banks of registers
*/
// Bank 0
#define PAF9701_PARTID_L                   0x00
#define PAF9701_PARTID_H                   0x01

#define PAF9701_ALERT_MODE                 0x03
#define PAF9701_OUTPUT_ENABLE              0x04
#define PAF9701_STATUS_FLAG                0x05
#define PAF9701_DSP_TO_DATA_L              0x06
#define PAF9701_DSP_TO_DATA_H              0x07

#define PAF9701_CAL_TO_DATA_L              0x0A
#define PAF9701_CAL_TO_DATA_H              0x0B

#define PAF9701_BURST_NUM_SEL              0x20
#define PAF9701_BURST_FRQ_SEL_L            0x21
#define PAF9701_BURST_FRQ_SEL_M            0x22
#define PAF9701_BURST_FRQ_SEL_H            0x23

#define PAF9701_ONE_SHOT_MODE              0x26
#define PAF9701_GPIO0_OPEN_DRAIN           0x27 // interrupt push pull (defualt) or open drain

#define PAF9701_DET1_RPT_RATE_L            0x65
#define PAF9701_DET1_RPT_RATE_M            0x66
#define PAF9701_DET1_RPT_RATE_H            0x67
#define PAF9701_DET2_RPT_RATE_L            0x68
#define PAF9701_DET2_RPT_RATE_M            0x69
#define PAF9701_DET2_RPT_RATE_H            0x6A

#define PAF9701_DET_TIME_L                 0x6B
#define PAF9701_DET_TIME_M                 0x6C
#define PAF9701_DET_TIME_H                 0x6D

#define PAF9701_OPERATION_MODE             0x7B

#define PAF9701_HOST_RSTB                  0x7D
#define PAF9701_POWER_SAVING_MODE          0x7E

//Bank 1
#define PAF9701_FILTER_SEL                 0x50

#define PAF9701_TA_HIGH_LIMIT_L            0x52
#define PAF9701_TA_HIGH_LIMIT_H            0x53
#define PAF9701_TA_LOW_LIMIT_L             0x54
#define PAF9701_TA_LOW_LIMIT_H             0x55
#define PAF9701_TO_HIGH_LIMIT_L            0x56
#define PAF9701_TO_HIGH_LIMIT_H            0x57
#define PAF9701_TO_LOW_LIMIT_L             0x58
#define PAF9701_TO_LOW_LIMIT_H             0x59
#define PAF9701_TA_HYSTERESIS              0x5A
#define PAF9701_TO_HYSTERESIS              0x5B

#define PAF9701_DET_TA_HIGH_LIMIT_L        0x5C
#define PAF9701_DET_TA_HIGH_LIMIT_H        0x5D
#define PAF9701_DET_TA_LOW_LIMIT_L         0x5E
#define PAF9701_DET_TA_LOW_LIMIT_H         0x5F
#define PAF9701_DET_TO_HIGH_LIMIT_L        0x60
#define PAF9701_DET_TO_HIGH_LIMIT_H        0x61
#define PAF9701_DET_TO_LOW_LIMIT_L         0x62
#define PAF9701_DET_TO_LOW_LIMIT_H         0x63
#define PAF9701_DET_TA_HYSTERESIS          0x64
#define PAF9701_DET_TO_HYSTERESIS          0x65

#define PAF9701_TO_PIXEL_THRESHOLD         0x67
#define PAF9701_DET_TO_PIXEL_THRESHOLD     0x68
#define PAF9701_TO_SKIP1_PIXEL_THRESHOLD   0x69
#define PAF9701_TO_SKIP2_PIXEL_THRESHOLD   0x6A

// Bank 3
#define PAF9701_EMISSIVITY_L               0x60
#define PAF9701_EMISSIVITY_M               0x61
#define PAF9701_EMISSIVITY_H               0x62
#define PAF9701_EMISSIVITY_UH              0x63
#define PAF9701_DECAY_TIME_L               0x64
#define PAF9701_DECAY_TIME_M               0x65
#define PAF9701_DECAY_TIME_H               0x66
#define PAF9701_DECAY_TIME_UH              0x67

#define PAF9701_ORIENTATION                0x6C

// Bank 4
#define PAF9701_TO_PIXEL_0_DATA_L          0x00
#define PAF9701_TO_PIXEL_0_DATA_H          0x01
#define PAF9701_TO_PIXEL_1_DATA_L          0x02
#define PAF9701_TO_PIXEL_1_DATA_H          0x03
#define PAF9701_TO_PIXEL_2_DATA_L          0x04
#define PAF9701_TO_PIXEL_2_DATA_H          0x05
#define PAF9701_TO_PIXEL_3_DATA_L          0x06
#define PAF9701_TO_PIXEL_3_DATA_H          0x07
#define PAF9701_TO_PIXEL_4_DATA_L          0x08
#define PAF9701_TO_PIXEL_4_DATA_H          0x09
#define PAF9701_TO_PIXEL_5_DATA_L          0x0A
#define PAF9701_TO_PIXEL_5_DATA_H          0x0B
#define PAF9701_TO_PIXEL_6_DATA_L          0x0C
#define PAF9701_TO_PIXEL_6_DATA_H          0x0D
#define PAF9701_TO_PIXEL_7_DATA_L          0x0E
#define PAF9701_TO_PIXEL_7_DATA_H          0x0F
#define PAF9701_TO_PIXEL_8_DATA_L          0x10
#define PAF9701_TO_PIXEL_8_DATA_H          0x11
#define PAF9701_TO_PIXEL_9_DATA_L          0x12
#define PAF9701_TO_PIXEL_9_DATA_H          0x13
#define PAF9701_TO_PIXEL_10_DATA_L         0x14
#define PAF9701_TO_PIXEL_10_DATA_H         0x15
#define PAF9701_TO_PIXEL_11_DATA_L         0x16
#define PAF9701_TO_PIXEL_11_DATA_H         0x17
#define PAF9701_TO_PIXEL_12_DATA_L         0x18
#define PAF9701_TO_PIXEL_12_DATA_H         0x19
#define PAF9701_TO_PIXEL_13_DATA_L         0x1A
#define PAF9701_TO_PIXEL_13_DATA_H         0x1B
#define PAF9701_TO_PIXEL_14_DATA_L         0x1C
#define PAF9701_TO_PIXEL_14_DATA_H         0x1D
#define PAF9701_TO_PIXEL_15_DATA_L         0x1E
#define PAF9701_TO_PIXEL_15_DATA_H         0x1F
#define PAF9701_TO_PIXEL_16_DATA_L         0x20
#define PAF9701_TO_PIXEL_16_DATA_H         0x21
#define PAF9701_TO_PIXEL_17_DATA_L         0x22
#define PAF9701_TO_PIXEL_17_DATA_H         0x23
#define PAF9701_TO_PIXEL_18_DATA_L         0x24
#define PAF9701_TO_PIXEL_18_DATA_H         0x25
#define PAF9701_TO_PIXEL_19_DATA_L         0x26
#define PAF9701_TO_PIXEL_19_DATA_H         0x27
#define PAF9701_TO_PIXEL_20_DATA_L         0x28
#define PAF9701_TO_PIXEL_20_DATA_H         0x29
#define PAF9701_TO_PIXEL_21_DATA_L         0x2A
#define PAF9701_TO_PIXEL_21_DATA_H         0x2B
#define PAF9701_TO_PIXEL_22_DATA_L         0x2C
#define PAF9701_TO_PIXEL_22_DATA_H         0x2D
#define PAF9701_TO_PIXEL_23_DATA_L         0x2E
#define PAF9701_TO_PIXEL_23_DATA_H         0x2F
#define PAF9701_TO_PIXEL_24_DATA_L         0x30
#define PAF9701_TO_PIXEL_24_DATA_H         0x31
#define PAF9701_TO_PIXEL_25_DATA_L         0x32
#define PAF9701_TO_PIXEL_25_DATA_H         0x33
#define PAF9701_TO_PIXEL_26_DATA_L         0x34
#define PAF9701_TO_PIXEL_26_DATA_H         0x35
#define PAF9701_TO_PIXEL_27_DATA_L         0x36
#define PAF9701_TO_PIXEL_27_DATA_H         0x37
#define PAF9701_TO_PIXEL_28_DATA_L         0x38
#define PAF9701_TO_PIXEL_28_DATA_H         0x39
#define PAF9701_TO_PIXEL_29_DATA_L         0x3A
#define PAF9701_TO_PIXEL_29_DATA_H         0x3B
#define PAF9701_TO_PIXEL_30_DATA_L         0x3C
#define PAF9701_TO_PIXEL_30_DATA_H         0x3D
#define PAF9701_TO_PIXEL_31_DATA_L         0x3E
#define PAF9701_TO_PIXEL_31_DATA_H         0x3F
#define PAF9701_TO_ALERT_FLAG_0_7          0x40
#define PAF9701_TO_ALERT_FLAG_8_15         0x41
#define PAF9701_TO_ALERT_FLAG_16_23        0x42
#define PAF9701_TO_ALERT_FLAG_24_31        0x43
#define PAF9701_TO_ALERT_FLAG_32_39        0x44
#define PAF9701_TO_ALERT_FLAG_40_47        0x45
#define PAF9701_TO_ALERT_FLAG_48_55        0x46
#define PAF9701_TO_ALERT_FLAG_56_63        0x47

#define PAF9701_P0_SELECT                  0x49
#define PAF9701_P0_WOI_V                   0x4A
#define PAF9701_P0_WOI_H                   0x4B
#define PAF9701_P1_SELECT                  0x4C
#define PAF9701_P1_WOI_V                   0x4D
#define PAF9701_P1_WOI_H                   0x4E
#define PAF9701_P2_SELECT                  0x4F
#define PAF9701_P2_WOI_V                   0x50
#define PAF9701_P2_WOI_H                   0x51
#define PAF9701_SKIP_MODE                  0x52

// Bank 5

#define PAF9701_TO_PIXEL_32_DATA_L         0x00
#define PAF9701_TO_PIXEL_32_DATA_H         0x01
#define PAF9701_TO_PIXEL_33_DATA_L         0x02
#define PAF9701_TO_PIXEL_33_DATA_H         0x03
#define PAF9701_TO_PIXEL_34_DATA_L         0x04
#define PAF9701_TO_PIXEL_34_DATA_H         0x05
#define PAF9701_TO_PIXEL_35_DATA_L         0x06
#define PAF9701_TO_PIXEL_35_DATA_H         0x07
#define PAF9701_TO_PIXEL_36_DATA_L         0x08
#define PAF9701_TO_PIXEL_36_DATA_H         0x09
#define PAF9701_TO_PIXEL_37_DATA_L         0x0A
#define PAF9701_TO_PIXEL_37_DATA_H         0x0B
#define PAF9701_TO_PIXEL_38_DATA_L         0x0C
#define PAF9701_TO_PIXEL_38_DATA_H         0x0D
#define PAF9701_TO_PIXEL_39_DATA_L         0x0E
#define PAF9701_TO_PIXEL_39_DATA_H         0x0F
#define PAF9701_TO_PIXEL_40_DATA_L         0x10
#define PAF9701_TO_PIXEL_40_DATA_H         0x11
#define PAF9701_TO_PIXEL_41_DATA_L         0x12
#define PAF9701_TO_PIXEL_41_DATA_H         0x13
#define PAF9701_TO_PIXEL_42_DATA_L         0x14
#define PAF9701_TO_PIXEL_42_DATA_H         0x15
#define PAF9701_TO_PIXEL_43_DATA_L         0x16
#define PAF9701_TO_PIXEL_43_DATA_H         0x17
#define PAF9701_TO_PIXEL_44_DATA_L         0x18
#define PAF9701_TO_PIXEL_44_DATA_H         0x19
#define PAF9701_TO_PIXEL_45_DATA_L         0x1A
#define PAF9701_TO_PIXEL_45_DATA_H         0x1B
#define PAF9701_TO_PIXEL_46_DATA_L         0x1C
#define PAF9701_TO_PIXEL_46_DATA_H         0x1D
#define PAF9701_TO_PIXEL_47_DATA_L         0x1E
#define PAF9701_TO_PIXEL_47_DATA_H         0x1F
#define PAF9701_TO_PIXEL_48_DATA_L         0x20
#define PAF9701_TO_PIXEL_48_DATA_H         0x21
#define PAF9701_TO_PIXEL_49_DATA_L         0x22
#define PAF9701_TO_PIXEL_49_DATA_H         0x23
#define PAF9701_TO_PIXEL_50_DATA_L         0x24
#define PAF9701_TO_PIXEL_50_DATA_H         0x25
#define PAF9701_TO_PIXEL_51_DATA_L         0x26
#define PAF9701_TO_PIXEL_51_DATA_H         0x27
#define PAF9701_TO_PIXEL_52_DATA_L         0x28
#define PAF9701_TO_PIXEL_52_DATA_H         0x29
#define PAF9701_TO_PIXEL_53_DATA_L         0x2A
#define PAF9701_TO_PIXEL_53_DATA_H         0x2B
#define PAF9701_TO_PIXEL_54_DATA_L         0x2C
#define PAF9701_TO_PIXEL_54_DATA_H         0x2D
#define PAF9701_TO_PIXEL_55_DATA_L         0x2E
#define PAF9701_TO_PIXEL_55_DATA_H         0x2F
#define PAF9701_TO_PIXEL_56_DATA_L         0x30
#define PAF9701_TO_PIXEL_56_DATA_H         0x31
#define PAF9701_TO_PIXEL_57_DATA_L         0x32
#define PAF9701_TO_PIXEL_57_DATA_H         0x33
#define PAF9701_TO_PIXEL_58_DATA_L         0x34
#define PAF9701_TO_PIXEL_58_DATA_H         0x35
#define PAF9701_TO_PIXEL_59_DATA_L         0x36
#define PAF9701_TO_PIXEL_59_DATA_H         0x37
#define PAF9701_TO_PIXEL_60_DATA_L         0x38
#define PAF9701_TO_PIXEL_60_DATA_H         0x39
#define PAF9701_TO_PIXEL_61_DATA_L         0x3A
#define PAF9701_TO_PIXEL_61_DATA_H         0x3B
#define PAF9701_TO_PIXEL_62_DATA_L         0x3C
#define PAF9701_TO_PIXEL_62_DATA_H         0x3D
#define PAF9701_TO_PIXEL_63_DATA_L         0x3E
#define PAF9701_TO_PIXEL_63_DATA_H         0x3F

#define PAF9701_BANK_SELECT                0x7F


#define PAF9701_ADDRESS  0x34  // if ADO is 0 (default), 0x57 if ADO == 1

enum runMode { // define run modes
 normal_mode     = 0x00,
 detection_mode1 = 0x20,
 detection_mode2 = 0x21,
 detection_mode3 = 0x22 
 };

enum imageFlip { // define image flip options
  noflipormirror = 0x00,
  flip           = 0x01,
  mirror         = 0x02,
  flipandmirror  = 0x03
};

enum imageRotate { // image rotate options
  orient0       =  0x00,
  orient90      =  0x01,
  orient180     =  0x02,
  orient270     =  0x03
};

enum digitalFilter {
  IIR           = 0x00, 
  movingAverage = 0x01,
  normalAverage = 0x02  // default
};

enum frameAverage {
 oneFrame       = 0x00,   // default
 twoFrames      = 0x01,
 fourFrames     = 0x02,
 eightFrames    = 0x03
};

enum IIRAverage {
  frames0_1     = 0x00, // default
  frames125_875 = 0x01, // fraction of frame n-1 + fraction of frame n
  frames250_750 = 0x02,
  frames375_625 = 0x03,
  frames500_500 = 0x04,
  frames625_375 = 0x05,
  frames750_250 = 0x06,
  frames825_125 = 0x07
};

enum alertMode {
 frameUpdateAlert   = 0x00,   // default
 absValueAlert      = 0x01,
 diffValueAlert     = 0x02
};


class PAF9701
{
  public: 
  PAF9701(I2Cdev* i2c_bus);
  uint16_t getChipID();
  void coldReset(); // reset all registers to default
  void warmReset(); // preserve register settings
  void initNormalMode(uint8_t runMode, uint32_t sampleRate, bool settle_en);
  void initAutoPowerSaveMode(bool detect3, uint32_t detectTime, bool settle_en);
  void suspendOperation();
  void resumeOperation();
  void clearInterrupt();
  uint8_t getStatus();
  void imageOrientation(uint8_t imageFlip, uint8_t imageRotate);
  int16_t getRawTaData();
  int16_t getCalTaData();
  uint8_t getPowerSaveMode();
  void getToData(float * temperatures);
  void setFilter(uint8_t digitalFilter, uint8_t frameAverage, uint8_t IIRAverage);
  void setAlertMode(uint8_t normalModeAlert, uint8_t det123ModeAlert);
  void setNormalAlertLimits(int16_t TaLow, int16_t TaHigh, uint8_t TaHyst, int16_t ToLow, int16_t ToHigh, int8_t ToHyst, uint8_t pixels);
  void setDet123AlertLimits(int16_t TaLow, int16_t TaHigh, uint8_t TaHyst, int16_t ToLow, int16_t ToHigh, int8_t ToHyst, uint8_t pixels);
  void getNormalAlertLimits(int16_t * output);
  void getAlertPixels(uint32_t * alertPixels);
  private:
  I2Cdev* _i2c_bus;
};

#endif
