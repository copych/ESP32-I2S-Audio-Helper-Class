// ===================== GLOBAL ======================================================================================
#define BOARD_HAS_UART_CHIP   // if your S3 board has just one USB port, you probably should comment this line out, as well as when using OTG port
#define DEBUG_ON              // note that debugging eats ticks initially belonging to real-time tasks, so sound output will be spoild in most cases, turn it off for production build

// ===================== Hardware Setup ======================================================================================
#ifndef LED_BUILTIN
#define LED_BUILTIN 0
#endif

#if (defined ARDUINO_LOLIN_S3_PRO)
#undef BOARD_HAS_UART_CHIP
#endif

#if (defined BOARD_HAS_UART_CHIP)
  #define MIDI_PORT_TYPE HardwareSerial
  #define MIDI_PORT Serial
  #define DEBUG_PORT Serial
#else
  #if (ESP_ARDUINO_VERSION_MAJOR < 3)
    #define MIDI_PORT_TYPE HWCDC
    #define MIDI_PORT USBSerial
    #define DEBUG_PORT USBSerial
  #else
    #define MIDI_PORT_TYPE HardwareSerial
    #define MIDI_PORT Serial
    #define DEBUG_PORT Serial
  #endif
#endif

// ===================== Debug Macros ======================================================================================
#ifdef DEBUG_ON
  #define DEB(...)    DEBUG_PORT.print(__VA_ARGS__) 
  #define DEBF(...)   DEBUG_PORT.printf(__VA_ARGS__)
  #define DEBUG(...)  DEBUG_PORT.println(__VA_ARGS__)
#else
  #define DEB(...)
  #define DEBF(...)
  #define DEBUG(...)
#endif
