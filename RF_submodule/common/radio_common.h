#pragma once

#define __STRUCT_PACKING __attribute__((__packed__))

/* Coordinate system for one robot */
typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t theta;
} __STRUCT_PACKING robot_position_t;

/* Parameters for a buoy */
typedef struct {
  uint16_t x;
  uint16_t y;
} __STRUCT_PACKING  buoy_position_t;

#define NUMBER_OF_FREE_BUOYS 12
#define NUMBER_OF_BUOY_PER_SKERRY 5
#define NUMBER_OF_SKERRY 4

#define SKERRY_BUOY_UNSURE 0
#define SKERRY_BUOY_RED 1
#define SKERRY_BUOY_GREEN 2
#define SKERRY_BUOY_GONE 3

/* Defines the structure for the radio packet */
typedef struct {
  /*  Flag field
   *  B7&6: Packet type (00: robots; 01: free buoys; 10: skerry buoys)
   *  B5: 1 if vane is detected
   *  B4: 1 if vane is north
   *  B3: blue first robot detected
   *  B2: blue second robot detected
   *  B1: yellow first robot detected
   *  B0: yellow second robot detected */
  uint16_t flags;

  union {
    
    struct {
      robot_position_t blue_first;
      robot_position_t blue_second;
      robot_position_t yellow_first;
      robot_position_t yellow_second;
    } __STRUCT_PACKING robots;

    buoy_position_t free_buoys[NUMBER_OF_FREE_BUOYS];

    uint8_t skerry_buoys[NUMBER_OF_SKERRY * NUMBER_OF_BUOY_PER_SKERRY];
       
  } content;
} __STRUCT_PACKING radio_payload_t;

/* Define payload length and statically assert it is less than the maximum size */
#define PAYLOAD_LENGTH sizeof(radio_payload_t)
static int static_size_assertion[(PAYLOAD_LENGTH <= 64) ? 1 : -1];

/* Default settings for SPI register */
static const uint8_t COMMON_RF_PARAMETERS[] = {
    /* RX/TX Parameters */
    0x01, 0x04, /* Standard mode */
    0x02, 0x02, /* FSK, packet, gaussian shaping */
    0x11, 0xFF, /* Full blast!!! */
    0x18, 0x08, /* 50 ohms antenna */
    0x07, 0xD9, /* 868 MHz */
    0x08, 0x00,
    0x09, 0x00,
    0x03, 0x03, /* Bit rate */
    0x04, 0x41,
    0x05, 0x02, /* Deviation in FSK */
    0x06, 0x00,
    0x12, 0x09, /* 40us PA ramp */
    /* Packet parameters */
    0x3C, 0x80, /* Starting TX on FIFO not empty */
    0x38, PAYLOAD_LENGTH,
    0x2C, 0x00, /* Preamble size */
    0x2D, 0x0A,
    0x37, 0x10, /* Fixed length, CRC on, no address filtering */
    0x2E, 0x98, /* Sync pattern on, 3 bytes */
    0x2F, 0x01, /* Sync bytes */
    0x30, 0x02,
    0x31, 0x03,
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
