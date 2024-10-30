// ===================== AUDIO ======================================================================================
#define   DMA_BUFFER_NUM        2     // number of internal DMA buffers
#define   DMA_BUFFER_LEN        32    // length of each buffer in samples
#define   CHANNEL_SAMPLE_BYTES  2     // can be 1, 2, 3 or 4 (2 and 4 only supported yet)
#define   SAMPLE_RATE           44100

// ===================== PINS ======================================================================================
#if defined(CONFIG_IDF_TARGET_ESP32S3)
// ESP32-S3 based boards:
#define I2S_BCLK_PIN    5       // I2S BIT CLOCK pin (BCL BCK CLK)
#define I2S_DOUT_PIN    6       // MCU Data Out: connect to periph. DATA IN (DIN D DAT)
#define I2S_WCLK_PIN    7       // I2S WORD CLOCK pin (WCK WCL LCK)
#define I2S_DIN_PIN     15      // MCU Data In: connect to periph. DATA OUT (DOUT D SD)
#elif defined(CONFIG_IDF_TARGET_ESP32)
// ESP32 based boards:
#define I2S_BCLK_PIN    5       // I2S BIT CLOCK pin (BCL BCK CLK)
#define I2S_DOUT_PIN    18      // MCU Data Out: connect to periph. DATA IN (DIN D DAT)
#define I2S_WCLK_PIN    19      // I2S WORD CLOCK pin (WCK WCL LCK)
#define I2S_DIN_PIN     23      // MCU Data In: connect to periph. DATA OUT (DOUT D SD)
#endif
