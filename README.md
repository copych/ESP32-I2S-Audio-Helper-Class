# ESP32-I2S-Audio-Helper-Class

A helper class to make I2S easy. Compatible with ESP Arduino cores 2.x.x and 3.x.x.


Access audio capabilities of ESP32 family with a few lines of code like

```
#include "i2s_config.h"
#include "i2s_in_out.h"

I2S_Audio AudioPort;

void setup() {
  AudioPort.init(I2S_Audio::MODE_IN_OUT);
}

void loop() {
  // get one input sample
  AudioPort.getSamples(sampleL, sampleR);

  // process it the way you like
  SomeOfYourEffects.process(&sampleL, &sampleR);

  // output processed sample
  AudioPort.putSamples(sampleL, sampleR);
}
```
