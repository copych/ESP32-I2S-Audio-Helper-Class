/**
 * I2S_Audio class usage example 
 * (c)2024 by Copych
 * - Board: "ESP32 Dev Module" or "ESP32S3 Dev Module"
 * - PSRAM: "Enabled" or "OPI PSRAM" or what type you have
 * 
*/

#include "config.h"
#include "i2s_config.h"
#include "i2s_in_out.h"
#include "fx_delay.h"
#include "fx_reverb.h"

// tasks for Core0 and Core1
TaskHandle_t Task1;
TaskHandle_t Task2;

// I2S
I2S_Audio AudioPort;

// Global effects
FxDelay Delay; 
FxReverb Reverb;  

/* 
 * Core Tasks ************************************************************************************************************************
*/
 
// Core0 task  
static void IRAM_ATTR audio_task1(void *userData) {
 
  DEBUG("\n Starting Task1");
  delay(10);
  
  float sampleL=0.0f;
  float sampleR=0.0f;

// manually managed buffer reading/writing
  while (true) {
    /*
    for (int j = 0; j < 1000; j++) {
      taskYIELD();     
      if (AudioPort.getReadSamplesRemain() == 0) AudioPort.readBuffer(); // cause we combine attended and unattended approaches
      for (int i = 0; i < AudioPort.getBufLenSmp(); i++) {
          sampleL = AudioPort.readSample(i, I2S_Audio::CH_LEFT);
          //sampleR = AudioPort.readSample(i, I2S_Audio::CH_RIGHT);
          sampleR = sampleL;
          
          Reverb.Process(&sampleL, &sampleR);
          Delay.Process(&sampleL, &sampleR);
          
          AudioPort.writeSample(sampleL, i, I2S_Audio::CH_LEFT);
          AudioPort.writeSample(sampleR, i, I2S_Audio::CH_RIGHT);
        
      }
      AudioPort.writeBuffer();
    }
*/
// unattended reading/writing
    for (int j = 0; j < 1000 * AudioPort.getBufLenSmp(); j++) {
      AudioPort.getSamples(sampleL, sampleR);
      sampleR = sampleL;
      Reverb.Process(&sampleL, &sampleR);
      Delay.Process(&sampleL, &sampleR);
      
      AudioPort.putSamples(sampleL, sampleR);
    }

  }
}

// task for Core1, which tipically runs user's code on ESP32
// static void IRAM_ATTR audio_task2(void *userData) {
static void IRAM_ATTR audio_task2(void *userData) {
 
  DEBUG("\n Starting Task2");
  delay(10);
  
  while (true) {
    taskYIELD();
    /*
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) { // wait for the notification from the SynthTask1
      taskYIELD();
      // EMPTY placeholder for the control routines on Core1
    }
    */
  }
}

/* 
 *  Quite an ordinary SETUP() *******************************************************************************************************************************
*/

void setup(void) {

#ifdef DEBUG_ON 
  DEBUG_PORT.begin(115200); 
#endif

  btStop(); // Stop Bluetooth: save cpu time for something else
 
  Reverb.Init();
  Delay.Init();
  AudioPort.init(I2S_Audio::MODE_IN_OUT);

  xTaskCreatePinnedToCore( audio_task1, "SynthTask1", 5000, NULL, 1, &Task1, 0 );
  xTaskCreatePinnedToCore( audio_task2, "SynthTask2", 5000, NULL, 1, &Task2, 1 );

  // somehow we should allow tasks to run
  xTaskNotifyGive(Task1);
  //  xTaskNotifyGive(Task2);
  DEBUG("\n SETUP() completed");
}

/* 
 *  Finally, the LOOP () ***********************************************************************************************************
*/

void loop() { // default loopTask running on the Core1
  // you can still place some of your code here
  // or   vTaskDelete(NULL);
  taskYIELD(); // this can wait
}
