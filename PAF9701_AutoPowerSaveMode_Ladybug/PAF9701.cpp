/* September 1, 2021 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 *  Simple library for configuring and reading data from PixArt Imaging's PAF9701
 *  8 x 8 pixel low-power thermal (IR) imaging camera.
 *  
 *  
 *  Library may be used freely and without limit with attribution.
 *  
 */
 
#include "PAF9701.h"
#include "I2CDev.h"

PAF9701::PAF9701(I2Cdev* i2c_bus)
 {
   _i2c_bus = i2c_bus;
 }


uint16_t PAF9701::getChipID()
 {
 _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
 uint8_t temp1 = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_PARTID_L);
 uint8_t temp2 = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_PARTID_H); // read part id registers 
 return ( ((uint16_t) temp2 << 8) | temp1);                             // report uint16_t result
 } 


void PAF9701::coldReset()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  // software reset the PAF9701, reset all registers to default
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_HOST_RSTB, 0x5A); 
 }


 void PAF9701::warmReset()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  // software reset the PAF9701, preserve register settings
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_HOST_RSTB, 0x9A); 
 }


  void PAF9701::initNormalMode(uint8_t runMode, uint32_t sampleRate, bool settle_en)
 {
  // initialize sensor to default values per 7.1.2 of the data sheet
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x7F, 0x00);  // select Bank 0
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x20, 0x79);  // Ta 2^7 samples, To 2^9 samples
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x21, 0x4E);  // 10 Hz, 97 ms conversion time
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x56, 0x33); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x1C, 0x03); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x79, 0x28); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x7A, 0x09); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x7F, 0x03);  // Bank 3
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x60, 0x48);  // 0.98 emmissivity
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x61, 0xE1); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x62, 0x7A); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x63, 0x3F); 

  // User-specified confguration
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  
  uint8_t temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE);
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE, temp & ~(0x10) ); // disable auto power save mode
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_OPERATION_MODE, runMode);  // select runMode
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_ONE_SHOT_MODE, 0x00);  // disable one-shot mode
  
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BURST_FRQ_SEL_L,  sampleRate & 0xFF);  // select sample rate
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BURST_FRQ_SEL_M, (sampleRate >> 8) & 0xFF);  // select sample rate
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BURST_FRQ_SEL_H, (sampleRate >> 16) & 0x0F);  // select sample rate
  
  temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE);
  if(settle_en){
    _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE, temp & ~(0x02) ); // enable settle function after sensor enable
  }
  else  {
    _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE, temp | 0x02 );    // disable settle function after sensor enable
  }
  
 }


   void PAF9701::initAutoPowerSaveMode(bool detect3, uint32_t detectTime, bool settle_en)
 {
  // initialize sensor to default values per 7.1.2 of the data sheet
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x7F, 0x00);  // select Bank 0
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x20, 0x79);  // Ta 2^7 samples, To 2^9 samples
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x21, 0x4E);  // 10 Hz, 97 ms conversion time
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x56, 0x33); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x1C, 0x03); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x79, 0x28); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x7A, 0x09); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x7F, 0x03);  // Bank 3
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x60, 0x48);  // 0.98 emmissivity
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x61, 0xE1); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x62, 0x7A); 
  _i2c_bus->writeByte(PAF9701_ADDRESS, 0x63, 0x3F); 

  // User-specified confguration
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  
  uint8_t temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE);
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE, temp | 0x10 ); // enable auto power save mode (bit 4)

  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x04);       // select Bank 4

  temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_SKIP_MODE);
  if(detect3) {
    _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_SKIP_MODE,  temp | 0x01);  // select skip mode, enable detect mode 3
  }
  else {
    _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_SKIP_MODE,  temp & ~(0x01));  // de-select skip mode, disable detect mode 3
  }

  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0

  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TIME_L,  detectTime & 0xFF);  // select sample rate
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TIME_M, (detectTime >> 8) & 0xFF);  // select sample rate
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TIME_H, (detectTime >> 16) & 0x0F);  // select sample rate

  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_ONE_SHOT_MODE, 0x00);  // disable one-shot mode
  
  temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE);
  if(settle_en){
    _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE, temp & ~(0x02) ); // enable settle function after sensor enable
  }
  else  {
    _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE, temp | 0x02 );    // disable settle function after sensor enable
  }
  
 }


  void PAF9701::suspendOperation()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_OUTPUT_ENABLE, 0x00); 
 }


  void PAF9701::resumeOperation()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_OUTPUT_ENABLE, 0x01); 
 }


  void PAF9701::clearInterrupt()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_STATUS_FLAG, 0x80);       // clear Frame_Update_flag
 }


  uint8_t PAF9701::getStatus()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  uint8_t temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_STATUS_FLAG);  // read status register
  return temp;
 }

 
  void PAF9701::imageOrientation(uint8_t imageFlip, uint8_t imageRotate)
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x03);       // select Bank 3
  // flip and rotate image
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_ORIENTATION, imageFlip << 2 | imageRotate);  // re-orient image frame
 }


  int16_t PAF9701::getRawTaData()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  // software reset the PAF9701, preserve register settings
  uint8_t temp1 = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_DSP_TO_DATA_L);  // read LSB register
  uint8_t temp2 = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_DSP_TO_DATA_H);  // read MSB register
  return ( ((int16_t) temp2 << 8) | temp1);
 }


  int16_t PAF9701::getCalTaData()
 {
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
  // software reset the PAF9701, preserve register settings
  uint8_t temp1 = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_CAL_TO_DATA_L);  // read LSB register
  uint8_t temp2 = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_CAL_TO_DATA_H);  // read MSB register
  return ( ((int16_t) temp2 << 8) | temp1);
 }

 
  uint8_t PAF9701::getPowerSaveMode()
 {
 _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
 uint8_t temp = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_POWER_SAVING_MODE);
 return temp;
 }


 void PAF9701::getToData(float * temperatures)
 {
  uint8_t rawData[128];
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x04);       // select Bank 4
  _i2c_bus->readBytes(PAF9701_ADDRESS, PAF9701_TO_PIXEL_0_DATA_L, 64, &rawData[0]);
  for(uint16_t ii = 0; ii < 32; ii++) {
    temperatures[ii] = (float) ((int16_t) ( (int16_t) rawData[2*ii + 1] << 8) | rawData[2*ii]);
    temperatures[ii] *=0.0625f; // scale to get temperatures in degrees C
  }
  
  _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x05);       // select Bank 5
  _i2c_bus->readBytes(PAF9701_ADDRESS, PAF9701_TO_PIXEL_32_DATA_L, 64, &rawData[64]);
  for(uint16_t ii = 32; ii < 64; ii++) {
    temperatures[ii] = (float) ((int16_t) ( (int16_t) rawData[2*ii + 1] << 8) | rawData[2*ii]);
    temperatures[ii] *=0.0625f; // scale to get temperatures in degrees C
  }
 }


 void PAF9701::setFilter(uint8_t digitalFilter, uint8_t frameAverage, uint8_t IIRAverage)
 {
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_FILTER_SEL, frameAverage << 5 | digitalFilter <<  3 | IIRAverage);        
 }


  void PAF9701::setAlertMode(uint8_t normalModeAlert, uint8_t det123ModeAlert)
 {
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x00);       // select Bank 0
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_ALERT_MODE, normalModeAlert << 2 | det123ModeAlert);        
 }
 

 void PAF9701::setNormalAlertLimits(int16_t TaLow, int16_t TaHigh, uint8_t TaHyst, int16_t ToLow, int16_t ToHigh, int8_t ToHyst, uint8_t pixels)
{
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x01);       // select Bank 1
   
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TA_LOW_LIMIT_L,  TaLow & 0x00FF);           // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TA_LOW_LIMIT_H, (TaLow & 0x0700) >> 8);     // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TA_HIGH_LIMIT_L,  TaHigh & 0x00FF);         // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TA_HIGH_LIMIT_H, (TaHigh & 0x0700) >> 8);   // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TO_LOW_LIMIT_L,  ToLow & 0x00FF);           // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TO_LOW_LIMIT_H, (ToLow & 0x0700) >> 8);     // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TO_HIGH_LIMIT_L,  ToHigh & 0x00FF);         // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TO_HIGH_LIMIT_H, (ToHigh & 0x0700) >> 8);   // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TA_HYSTERESIS, TaHyst);          
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TO_HYSTERESIS, ToHyst);   
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_TO_PIXEL_THRESHOLD, pixels);   
}


 void PAF9701::setDet123AlertLimits(int16_t TaLow, int16_t TaHigh, uint8_t TaHyst, int16_t ToLow, int16_t ToHigh, int8_t ToHyst, uint8_t pixels)
{
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x01);       // select Bank 1
   
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TA_LOW_LIMIT_L,  TaLow & 0x00FF);           // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TA_LOW_LIMIT_H, (TaLow & 0x0700) >> 8);     // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TA_HIGH_LIMIT_L,  TaHigh & 0x00FF);         // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TA_HIGH_LIMIT_H, (TaHigh & 0x0700) >> 8);   // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TO_LOW_LIMIT_L,  ToLow & 0x00FF);           // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TO_LOW_LIMIT_H, (ToLow & 0x0700) >> 8);     // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TO_HIGH_LIMIT_L,  ToHigh & 0x00FF);         // LSB only
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TO_HIGH_LIMIT_H, (ToHigh & 0x0700) >> 8);   // upper first 3 bits only     
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TA_HYSTERESIS, TaHyst);          
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TO_HYSTERESIS, ToHyst);   
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_DET_TO_PIXEL_THRESHOLD, pixels);   
}


 void PAF9701::getNormalAlertLimits(int16_t * output)
{
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x01);       // select Bank 1

   uint8_t rawData[2] = {0, 0};
   rawData[0] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TA_LOW_LIMIT_L);      
   rawData[1] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TA_LOW_LIMIT_H);  
   output[0] = (int16_t) (rawData[1] << 8) | rawData[0]; 
   rawData[0] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TA_HIGH_LIMIT_L);      
   rawData[1] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TA_HIGH_LIMIT_H);  
   output[1] = (int16_t) (rawData[1] << 8) | rawData[0]; 
   rawData[0] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TO_LOW_LIMIT_L);      
   rawData[1] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TO_LOW_LIMIT_H);  
   output[2] = (int16_t) (rawData[1] << 8) | rawData[0]; 
   rawData[0] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TO_HIGH_LIMIT_L);      
   rawData[1] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TO_HIGH_LIMIT_H);  
   output[3] = (int16_t) (rawData[1] << 8) | rawData[0]; 
   output[4] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TA_HYSTERESIS); 
   output[5] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TO_HYSTERESIS); 
   output[6] = _i2c_bus->readByte(PAF9701_ADDRESS, PAF9701_TO_PIXEL_THRESHOLD); 
}


void PAF9701::getAlertPixels(uint32_t * alertPixels)
{
   _i2c_bus->writeByte(PAF9701_ADDRESS, PAF9701_BANK_SELECT, 0x04);       // select Bank 4

   uint8_t rawData[4] = {0, 0, 0, 0};
   _i2c_bus->readBytes(PAF9701_ADDRESS, PAF9701_TO_ALERT_FLAG_0_7, 4, &rawData[0]); 
   alertPixels[0] = ((uint32_t) rawData[3] << 24) | ((uint32_t) rawData[2] << 16) | ((uint32_t) rawData[1] << 8) | rawData[0];
   _i2c_bus->readBytes(PAF9701_ADDRESS, PAF9701_TO_ALERT_FLAG_32_39, 4, &rawData[0]); 
   alertPixels[1] = ((uint32_t) rawData[3] << 24) | ((uint32_t) rawData[2] << 16) | ((uint32_t) rawData[1] << 8) | rawData[0];
}

 
