#include "i2s_in_out.h"

void I2S_Audio::init(eI2sMode select_mode) {
  _i2s_mode = select_mode;
  
  i2s_mode_t port_mode;
  
/*
  malloc_caps hints
      MALLOC_CAP_EXEC     Memory must be able to run executable code
      MALLOC_CAP_32BIT    Memory must allow for aligned 32-bit data accesses
      MALLOC_CAP_8BIT     Memory must allow for 8/16/...-bit data accesses
      MALLOC_CAP_DMA      Memory must be able to be accessed by DMA
      MALLOC_CAP_SPIRAM   Memory must be in SPI RAM
      MALLOC_CAP_INTERNAL Memory must be internal; specifically it should not disappear when flash/spiram cache is switched off
      MALLOC_CAP_DEFAULT  Memory can be returned in a non-capability-specific memory allocation (e.g. malloc(), calloc()) call
      MALLOC_CAP_INVALID  Memory can't be used / list end marker
*/
#if (CHANNEL_SAMPLE_BYTES == 4)
  uint32_t malloc_caps = (i2s_mode_t)( MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT );
#else
  uint32_t malloc_caps = (i2s_mode_t)( MALLOC_CAP_INTERNAL| MALLOC_CAP_8BIT );
#endif
  switch(_i2s_mode) {
    case MODE_IN:
    #ifndef USE_V3
      port_mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
    #endif
      pinMode(I2S_DIN_PIN, INPUT);
      _input_buf = (BUF_TYPE*)heap_caps_malloc( _buffer_size , malloc_caps );
      if (_input_buf == NULL) {
        DEBUG("I2S_Audio: Couldn't allocate memory for I2S input buffer"); 
      } else { 
        DEBF("I2S_Audio: I2S input buffer allocated %d bytes, &=%#010x\r\n", _buffer_size, _input_buf); 
        memset(_input_buf, 0, _buffer_size);
      }      
      break;
    case MODE_IN_OUT:
    #ifndef USE_V3
      port_mode = (i2s_mode_t)( I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX );
    #endif
      pinMode(I2S_DOUT_PIN, OUTPUT);
      pinMode(I2S_DIN_PIN, INPUT);
      _input_buf = (BUF_TYPE*)heap_caps_malloc( _buffer_size , malloc_caps );
      if (_input_buf == NULL) {
        DEBUG("I2S_Audio: Couldn't allocate memory for I2S input buffer"); 
      } else { 
        DEBF("I2S_Audio: I2S input buffer allocated %d bytes, &=%#010x\r\n", _buffer_size, _input_buf); 
        memset(_input_buf, 0, _buffer_size);
      }
      _output_buf = (BUF_TYPE*)heap_caps_malloc( _buffer_size , malloc_caps );
      if (_output_buf == NULL) {
        DEBUG("I2S_Audio: Couldn't allocate memory for I2S output buffer"); 
      } else { 
        DEBF("I2S_Audio: I2S output buffer allocated %d bytes, &=%#010x\r\n", _buffer_size, _output_buf); 
        memset(_output_buf, 0, _buffer_size);
      }
      break;
    case MODE_OUT:
    default:
    #ifndef USE_V3
      port_mode = (i2s_mode_t)( I2S_MODE_MASTER | I2S_MODE_TX );
    #endif
      pinMode(I2S_DOUT_PIN, OUTPUT);
      _output_buf = (BUF_TYPE*)heap_caps_malloc( _buffer_size , malloc_caps );
      if (_output_buf == NULL) {
        DEBUG("Couldn't allocate memory for I2S output buffer"); 
      } else { 
        DEBF("I2S_Audio: I2S output buffer allocated %d bytes, &=%#010x\r\n", _buffer_size, _output_buf); 
        memset(_output_buf, 0, _buffer_size);
      }

  }
  pinMode(I2S_BCLK_PIN, OUTPUT);
  pinMode(I2S_WCLK_PIN, OUTPUT);

#ifdef USE_V3
  I2S.setPins(I2S_BCLK_PIN, I2S_WCLK_PIN, I2S_DOUT_PIN, I2S_DIN_PIN); //SCK, WS, SDOUT, SDIN, MCLK
  I2S.begin(I2S_MODE_STD, _sample_rate, chn_bit_width, I2S_SLOT_MODE_STEREO);
#else
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)port_mode,
  //  .mode = (i2s_mode_t)( I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX ),
    .sample_rate = _sample_rate,
    .bits_per_sample = chn_bit_width,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S ),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUFFER_NUM,
    .dma_buf_len = DMA_BUFFER_LEN,
    .use_apll = true,
    .tx_desc_auto_clear = true,
  //  .fixed_mclk = 0
	//	.fixed_mclk = (int)(_sample_rate * mclk_multiplier),
  };

  i2s_pin_config_t i2s_pin_config = {
    .bck_io_num     = I2S_BCLK_PIN,
    .ws_io_num      = I2S_WCLK_PIN,
    .data_out_num   = I2S_DOUT_PIN,
    .data_in_num    = I2S_DIN_PIN
  };

  int err = i2s_driver_install(_i2s_port, &i2s_config, 0, NULL);
  i2s_set_pin(_i2s_port, &i2s_pin_config);
  i2s_zero_dma_buffer(_i2s_port);
  if (err == ESP_OK) {
    DEBF ("I2S_Audio: I2S started OK at BCLK %d, WCLK %d, DIN %d, DOUT %d\r\n", I2S_BCLK_PIN, I2S_WCLK_PIN, I2S_DIN_PIN, I2S_DOUT_PIN);
    DEBF ("I2S_Audio: int_to_float %10.10f, float_to_int %10.10f\r\n", int_to_float, float_to_int );
  } else { 
    DEBF("I2S_Audio: I2S Install Error %d", err);
  }
#endif
}

void I2S_Audio::deInit() {
  free(_input_buf);
  free(_output_buf);
#ifdef USE_V3  
  I2S.end();
#else
  i2s_zero_dma_buffer(_i2s_port);
  i2s_driver_uninstall(_i2s_port);
#endif
}


void I2S_Audio::readBuffer(BUF_TYPE* buf) {
#ifdef USE_V3
  I2S.readBytes((char*)buf, _buffer_size);  
  _read_remain_smp = DMA_BUFFER_LEN;
#else
  uint32_t bytesRead = 0;
	int32_t err = i2s_read(_i2s_port, (void*) buf, _buffer_size, &bytesRead, portMAX_DELAY);

	if (err || bytesRead < _buffer_size) {
		DEB("\nI2S_Audio: I2S read error: ");
		DEBUG(bytesRead);
	}
  _read_remain_smp = bytesRead / WHOLE_SAMPLE_BYTES;
#endif
}

void I2S_Audio::writeBuffer(BUF_TYPE* buf){
#ifdef USE_V3
  I2S.write((uint8_t*)buf, _buffer_size);
  _write_remain_smp = _buffer_len;
#else
	size_t bytesWritten = 0;
	int32_t err = i2s_write(_i2s_port, (char *)buf, _buffer_size, &bytesWritten, portMAX_DELAY);
  _write_remain_smp = bytesWritten / WHOLE_SAMPLE_BYTES;
  if (err != ESP_OK) {DEBUG("\nI2S_Audio: Error writing to port");  }
#endif
}

void I2S_Audio::getSamples(float* sampleLeft, float* sampleRight){
  getSamples(sampleLeft, sampleRight, _input_buf );
}

void I2S_Audio::getSamples(float& sampleLeft, float& sampleRight){
  getSamples(sampleLeft, sampleRight, _input_buf );  
}

void I2S_Audio::getSamples(float* sampleLeft, float* sampleRight, BUF_TYPE* buf ){
  int n = DMA_BUFFER_LEN - _read_remain_smp;
#if AUDIO_CHANNEL_NUM == 2
  *sampleLeft = convertInSample(_input_buf[AUDIO_CHANNEL_NUM * n ]);
  *sampleRight = convertInSample(_input_buf[AUDIO_CHANNEL_NUM * n + 1]);
#else
  *sampleLeft = convertInSample(_input_buf[AUDIO_CHANNEL_NUM * n + chan]);
#endif
  _read_remain_smp--;
  if (_read_remain_smp <= 0) {
    readBuffer(buf);
  } 
}

void I2S_Audio::getSamples(float& sampleLeft, float& sampleRight, BUF_TYPE* buf ){  
  int n = DMA_BUFFER_LEN - _read_remain_smp;
#if AUDIO_CHANNEL_NUM == 2
  sampleLeft = convertInSample(_input_buf[AUDIO_CHANNEL_NUM * n ]);
  sampleRight = convertInSample(_input_buf[AUDIO_CHANNEL_NUM * n + 1]);
#else
  sampleLeft = convertInSample(_input_buf[AUDIO_CHANNEL_NUM * n + chan]);
#endif
  _read_remain_smp--;
  if (_read_remain_smp <= 0) {
    readBuffer(buf);
  } 
}

void I2S_Audio::putSamples(float* sampleLeft, float* sampleRight){
  putSamples(sampleLeft, sampleRight, _output_buf );
}

void I2S_Audio::putSamples(float& sampleLeft, float& sampleRight){
  putSamples(sampleLeft, sampleRight, _output_buf );
}

void I2S_Audio::putSamples(float* sampleLeft, float* sampleRight, BUF_TYPE* buf ){
  int n = DMA_BUFFER_LEN - _write_remain_smp;
#if AUDIO_CHANNEL_NUM == 2
  buf[AUDIO_CHANNEL_NUM * n ] = convertOutSample(*sampleLeft);
  buf[AUDIO_CHANNEL_NUM * n + 1] = convertOutSample(*sampleRight);
#else
  buf[ n ] = convertOutSample(*sampleLeft);
#endif
  _write_remain_smp--;
  if (_write_remain_smp <= 0) {
    writeBuffer(buf);
  }  
}

void I2S_Audio::putSamples(float& sampleLeft, float& sampleRight, BUF_TYPE* buf ){
  int n = DMA_BUFFER_LEN - _write_remain_smp;
#if AUDIO_CHANNEL_NUM == 2
  buf[AUDIO_CHANNEL_NUM * n ] = convertOutSample(sampleLeft);
  buf[AUDIO_CHANNEL_NUM * n + 1] = convertOutSample(sampleRight);
#else
  buf[ n ] = convertOutSample(sampleLeft);
#endif
  _write_remain_smp--;
  if (_write_remain_smp <= 0) {
    writeBuffer(buf);
  }  
}
