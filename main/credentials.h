/*

Credentials file

*/

#pragma once

// Only one of these settings must be defined
#define USE_ABP
//#define USE_OTAA

#ifdef USE_ABP
    
    // LoRaWAN NwkSKey, network session key
    // This is the default Semtech key, which is used by the early prototype TTN
    // network.
    static const PROGMEM u1_t NWKSKEY[16] = { 0x0A, 0xDB, 0x33, 0x6D, 0x27, 0x41, 0xA4, 0xE5, 0x56, 0x6D, 0xA0, 0x34, 0x9C, 0x1A, 0x85, 0x03 };

    // LoRaWAN AppSKey, application session key
    // This is the default Semtech key, which is used by the early prototype TTN
    // network.
    static const u1_t PROGMEM APPSKEY[16] = { 0x35, 0xFC, 0xEA, 0xDC, 0x9D, 0xEB, 0x61, 0xE7, 0x3F, 0x28, 0x1B, 0xBA, 0x42, 0xCB, 0x9B, 0xE7 };
    
    // LoRaWAN end-device address (DevAddr)
    // This has to be unique for every node
    static const u4_t DEVADDR = 0x260110C6;

#endif

#ifdef USE_OTAA

    // This EUI must be in little-endian format, so least-significant-byte
    // first. When copying an EUI from ttnctl output, this means to reverse
    // the bytes. For TTN issued EUIs the last bytes should be 0x00, 0x00,
    // 0x00.
    static const u1_t PROGMEM APPEUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // This should also be in little endian format, see above.
    static const u1_t PROGMEM DEVEUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // This key should be in big endian format (or, since it is not really a
    // number but a block of memory, endianness does not really apply). In
    // practice, a key taken from ttnctl can be copied as-is.
    // The key shown here is the semtech default key.
    static const u1_t PROGMEM APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#endif
