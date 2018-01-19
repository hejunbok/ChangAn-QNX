#include <stdio.h>
#include <math.h>
#include <errno.h>

#include "audio_hero.h"

extern int audio_data_conv ( int val, short int *audio_y_data );
extern int audio_calc_x ( double val );
extern short int audio_calc_y ( double val );
extern int audio_write_x_reg ( const int addr, const int *data, int data_num );
extern int audio_write_y_reg ( const int addr, short int *data, int data_num );
extern int audio_send ( unsigned char *data, const int data_num );
extern int audio_init ( void );

#define XRAM_CLICK_SIZE                  171
#define XRAM_CLACK_SIZE                  171

static const audio_chime_addr audio_chime_addrs[SLOT_CNT] = {
  {AUDIO_Y_Tsq_T1b1Htone_Bk, AUDIO_Y_Tsq_T1b1Ltone_Bk, AUDIO_X_Tsq_T1_Bk, AUDIO_Y_Tsq_T1Vtone_Bk, AUDIO_Y_Tsq_T1Type_Bk, AUDIO_Y_Tsq_T1Clip_Bk},
  {AUDIO_Y_Tsq_T2b1Htone_Bk, AUDIO_Y_Tsq_T2b1Ltone_Bk, AUDIO_X_Tsq_T2_Bk, AUDIO_Y_Tsq_T2Vtone_Bk, AUDIO_Y_Tsq_T2Type_Bk, AUDIO_Y_Tsq_T2Clip_Bk},
  {AUDIO_Y_Tsq_T3b1Htone_Bk, AUDIO_Y_Tsq_T3b1Ltone_Bk, AUDIO_X_Tsq_T3_Bk, AUDIO_Y_Tsq_T3Vtone_Bk, AUDIO_Y_Tsq_T3Type_Bk, AUDIO_Y_Tsq_T3Clip_Bk},
  {AUDIO_Y_Tsq_T4b1Htone_Bk, AUDIO_Y_Tsq_T4b1Ltone_Bk, AUDIO_X_Tsq_T4_Bk, AUDIO_Y_Tsq_T4Vtone_Bk, AUDIO_Y_Tsq_T4Type_Bk, AUDIO_Y_Tsq_T4Clip_Bk}
};

static const char audio_chime_data[] = {
  (char)(3 * 61),
  (char)0xF2, (char)0x07, (char)0x61,  (char)0x08, (char)0x89, (char)0x86,  (char)0x07, (char)0x88, (char)0x87,  (char)0x08, (char)0x88, (char)0x86,  (char)0x07, (char)0x87, (char)0x86,
  (char)0x0A, (char)0x8A, (char)0x87,  (char)0x0A, (char)0x89, (char)0x89,  (char)0x08, (char)0x88, (char)0x89,  (char)0x08, (char)0x88, (char)0x88,  (char)0x0A, (char)0x89, (char)0x8A,
  (char)0x0C, (char)0x8C, (char)0x8B,  (char)0x07, (char)0x87, (char)0x89,  (char)0x0F, (char)0x8D, (char)0x89,  (char)0x05, (char)0x89, (char)0x8D,  (char)0x0E, (char)0x88, (char)0x84,
  (char)0x0A, (char)0x8E, (char)0x8E,  (char)0x0A, (char)0x89, (char)0x87,  (char)0x19, (char)0x94, (char)0x90,  (char)0x0C, (char)0x9A, (char)0x9F,  (char)0xF7, (char)0x6E, (char)0x7A,
  (char)0x27, (char)0xA4, (char)0x8E,  (char)0xF0, (char)0x75, (char)0x91,  (char)0x39, (char)0xA2, (char)0x84,  (char)0xEB, (char)0x86, (char)0xAC,  (char)0x08, (char)0x79, (char)0x67,
  (char)0x0C, (char)0x95, (char)0x91,  (char)0x13, (char)0xA6, (char)0x99,  (char)0xDE, (char)0x56, (char)0x71,  (char)0x48, (char)0xB5, (char)0x87,  (char)0xBD, (char)0x55, (char)0x9F,
  (char)0x77, (char)0xBA, (char)0x6D,  (char)0xD4, (char)0xAE, (char)0xF8,  (char)0x10, (char)0x57, (char)0x33,  (char)0x1C, (char)0xB8, (char)0xB0,  (char)0xE3, (char)0x62, (char)0x69,
  (char)0x13, (char)0x75, (char)0x63,  (char)0x03, (char)0xAA, (char)0xB5,  (char)0xC5, (char)0x35, (char)0x5D,  (char)0x50, (char)0xC9, (char)0x91,  (char)0xDF, (char)0x84, (char)0xB1,
  (char)0x26, (char)0x8C, (char)0x66,  (char)0x34, (char)0xB5, (char)0xB2,  (char)0xDC, (char)0x71, (char)0x97,  (char)0x28, (char)0x69, (char)0x4B,  (char)0xF4, (char)0xB9, (char)0xCE,
  (char)0xC8, (char)0x2E, (char)0x40,  (char)0x36, (char)0xD1, (char)0x9F,  (char)0xAC, (char)0x48, (char)0x80,  (char)0x21, (char)0x6A, (char)0x35,  (char)0x11, (char)0xA6, (char)0xAF,
  (char)0xF6, (char)0x5F, (char)0x74,  (char)0x41, (char)0xAC, (char)0x9A,  (char)0x07, (char)0xA4, (char)0xC3,  (char)0x0E, (char)0x82, (char)0x79,  (char)0x1E, (char)0xB8, (char)0xA0,
  (char)0xBE, (char)0x56, (char)0x72,  (char)0xEE, (char)0x4D, (char)0x3B,  (char)0xDB, (char)0x68, (char)0x7A,  (char)0xD7, (char)0x3D, (char)0x45,  (char)0x2E, (char)0xB2, (char)0x8B,
  (char)0xE9, (char)0x7C, (char)0x9B,
  (char)(3 * 61),
  (char)0xF2, (char)0x07, (char)0x9D,  (char)0x4B, (char)0xB3, (char)0x87,  (char)0x07, (char)0x9F, (char)0xC1,  (char)0x20, (char)0x8B, (char)0x7F,  (char)0xF0, (char)0x8A, (char)0xA1,
  (char)0xEF, (char)0x6C, (char)0x69,  (char)0xE2, (char)0x74, (char)0x7A,  (char)0xE5, (char)0x59, (char)0x57,  (char)0xED, (char)0x6C, (char)0x68,  (char)0xE6, (char)0x5E, (char)0x5F,
  (char)0x0C, (char)0x7D, (char)0x6F,  (char)0x0F, (char)0x98, (char)0x98,  (char)0x10, (char)0x8A, (char)0x8E,  (char)0x29, (char)0xAE, (char)0xA3,  (char)0xFB, (char)0x88, (char)0x95,
  (char)0x19, (char)0x8C, (char)0x79,  (char)0xEC, (char)0x81, (char)0x94,  (char)0xE2, (char)0x54, (char)0x5A,  (char)0xE1, (char)0x71, (char)0x74,  (char)0xDB, (char)0x52, (char)0x54,
  (char)0xFC, (char)0x80, (char)0x6F,  (char)0xF9, (char)0x73, (char)0x76,  (char)0x21, (char)0x9C, (char)0x8D,  (char)0x0F, (char)0x97, (char)0xA0,  (char)0x20, (char)0x91, (char)0x8C,
  (char)0x18, (char)0x9F, (char)0xA4,  (char)0xF4, (char)0x74, (char)0x85,  (char)0x02, (char)0x84, (char)0x7F,  (char)0xD8, (char)0x68, (char)0x79,  (char)0xEC, (char)0x5E, (char)0x52,
  (char)0xEC, (char)0x73, (char)0x6F,  (char)0xF2, (char)0x69, (char)0x65,  (char)0x00, (char)0x7F, (char)0x7C,  (char)0x12, (char)0x87, (char)0x82,  (char)0x1D, (char)0x9E, (char)0x9D,
  (char)0x18, (char)0x96, (char)0x9A,  (char)0x17, (char)0x9B, (char)0x9B,  (char)0xF9, (char)0x83, (char)0x8D,  (char)0xF6, (char)0x6F, (char)0x71,  (char)0xEF, (char)0x76, (char)0x79,
  (char)0xDB, (char)0x5B, (char)0x65,  (char)0xF1, (char)0x6B, (char)0x63,  (char)0xF8, (char)0x78, (char)0x75,  (char)0x07, (char)0x83, (char)0x7D,  (char)0x16, (char)0x90, (char)0x8A,
  (char)0x13, (char)0x96, (char)0x97,  (char)0x0C, (char)0x8A, (char)0x8D,  (char)0x10, (char)0x94, (char)0x92,  (char)0xF6, (char)0x7D, (char)0x87,  (char)0xF8, (char)0x77, (char)0x74,
  (char)0xF0, (char)0x72, (char)0x75,  (char)0xED, (char)0x6D, (char)0x6F,  (char)0xF5, (char)0x70, (char)0x6E,  (char)0xFB, (char)0x7C, (char)0x7B,  (char)0x01, (char)0x7B, (char)0x7A,
  (char)0x0F, (char)0x8D, (char)0x87,  (char)0x0A, (char)0x8B, (char)0x8D,  (char)0x0E, (char)0x8B, (char)0x89,  (char)0x05, (char)0x89, (char)0x8E,  (char)0x04, (char)0x82, (char)0x81,
  (char)0xFA, (char)0x82, (char)0x85,
  (char)(3 * 61),
  (char)0xF2, (char)0x07, (char)0xD9,  (char)0xF0, (char)0x6F, (char)0x73,  (char)0xF5, (char)0x75, (char)0x73,  (char)0xEE, (char)0x6F, (char)0x73,  (char)0xFD, (char)0x76, (char)0x72,
  (char)0xFF, (char)0x82, (char)0x81,  (char)0x02, (char)0x7F, (char)0x7D,  (char)0x0A, (char)0x8A, (char)0x87,  (char)0x07, (char)0x88, (char)0x89,  (char)0x05, (char)0x86, (char)0x87,
  (char)0xFD, (char)0x7F, (char)0x83,  (char)0xF8, (char)0x78, (char)0x7B,  (char)0xF8, (char)0x78, (char)0x78,  (char)0xF6, (char)0x78, (char)0x79,  (char)0xF8, (char)0x76, (char)0x75,
  (char)0xFE, (char)0x7D, (char)0x7A,  (char)0xFE, (char)0x7D, (char)0x7D,  (char)0x02, (char)0x82, (char)0x80,  (char)0x01, (char)0x81, (char)0x81,  (char)0x00, (char)0x81, (char)0x81,
  (char)0x00, (char)0x7F, (char)0x7F,  (char)0x02, (char)0x83, (char)0x81,  (char)0xFD, (char)0x7F, (char)0x81,  (char)0xFE, (char)0x7D, (char)0x7D,  (char)0xFC, (char)0x7C, (char)0x7E,
  (char)0xFC, (char)0x7B, (char)0x7B,  (char)0xFE, (char)0x7E, (char)0x7D,  (char)0xFC, (char)0x7D, (char)0x7E,  (char)0xFF, (char)0x7D, (char)0x7C,  (char)0x00, (char)0x80, (char)0x80,
  (char)0x00, (char)0x7F, (char)0x7F,  (char)0x02, (char)0x83, (char)0x82,  (char)0x00, (char)0x81, (char)0x82,  (char)0x00, (char)0x7F, (char)0x7F,  (char)0x00, (char)0x81, (char)0x81,
  (char)0xFE, (char)0x7D, (char)0x7F,  (char)0xFF, (char)0x7F, (char)0x7E,  (char)0xFD, (char)0x7D, (char)0x7E,  (char)0xFF, (char)0x7F, (char)0x7D,  (char)0xFE, (char)0x7F, (char)0x7F,
  (char)0xFF, (char)0x7E, (char)0x7D,  (char)0x00, (char)0x80, (char)0x7F,  (char)0x00, (char)0x80, (char)0x80,  (char)0x01, (char)0x81, (char)0x81,  (char)0x02, (char)0x82, (char)0x82,
  (char)0x01, (char)0x81, (char)0x82,  (char)0x01, (char)0x81, (char)0x81,  (char)0xFF, (char)0x80, (char)0x81,  (char)0xFF, (char)0x7E, (char)0x7E,  (char)0xFF, (char)0x7F, (char)0x7F,
  (char)0xFE, (char)0x7F, (char)0x7E,  (char)0x00, (char)0x7F, (char)0x7F,  (char)0x02, (char)0x82, (char)0x82,  (char)0x07, (char)0x83, (char)0x84,  (char)0x06, (char)0x84, (char)0x85,
  (char)0x04, (char)0x86, (char)0x84,  (char)0x04, (char)0x85, (char)0x84,  (char)0x04, (char)0x83, (char)0x82,  (char)0x03, (char)0x83, (char)0x84,  (char)0x02, (char)0x83, (char)0x82,
  (char)0x03, (char)0x84, (char)0x84,
  (char)(3 * 61),
  (char)0xF2, (char)0x08, (char)0x15,  (char)0x01, (char)0x80, (char)0x84,  (char)0x05, (char)0x83, (char)0x82,  (char)0x04, (char)0x86, (char)0x85,  (char)0xF6, (char)0x79, (char)0x7F,
  (char)0x0D, (char)0x85, (char)0x7A,  (char)0xFC, (char)0x85, (char)0x8E,  (char)0x02, (char)0x7B, (char)0x7A,  (char)0xFC, (char)0x7D, (char)0x82,  (char)0x02, (char)0x83, (char)0x7B,
  (char)0x0A, (char)0x83, (char)0x82,  (char)0x06, (char)0x9F, (char)0xA2,  (char)0xD3, (char)0x40, (char)0x5C,  (char)0x2A, (char)0xB0, (char)0x80,  (char)0xD2, (char)0x52, (char)0x7E,
  (char)0x4B, (char)0xB8, (char)0x89,  (char)0xCA, (char)0x66, (char)0xA5,  (char)0xF0, (char)0x76, (char)0x5C,  (char)0x05, (char)0x81, (char)0x77,  (char)0xD6, (char)0x95, (char)0x9D,
  (char)0xF8, (char)0x36, (char)0x23,  (char)0x22, (char)0xD2, (char)0xB8,  (char)0xDA, (char)0x3F, (char)0x59,  (char)0x37, (char)0xC1, (char)0x99,  (char)0x0E, (char)0x9E, (char)0xA7,
  (char)0xDD, (char)0x63, (char)0x73,  (char)0x45, (char)0xA5, (char)0x6A,  (char)0x9B, (char)0x58, (char)0x95,  (char)0x1D, (char)0x4F, (char)0x0A,  (char)0xE0, (char)0x82, (char)0xA8,
  (char)0xEC, (char)0x30, (char)0x38,  (char)0x35, (char)0x9B, (char)0x90,  (char)0xDD, (char)0x89, (char)0xAE,  (char)0x0F, (char)0x55, (char)0x46,  (char)0x58, (char)0xF1, (char)0xD7,
  (char)0xBF, (char)0x4A, (char)0x94,  (char)0x30, (char)0x78, (char)0x4B,  (char)0xE6, (char)0xA0, (char)0xBE,  (char)0xB0, (char)0x0C, (char)0x2F,  (char)0x37, (char)0xB3, (char)0x85,
  (char)0x83, (char)0x3B, (char)0x91,  (char)0x10, (char)0x4B, (char)0x0F,  (char)0x1B, (char)0xC7, (char)0xB8,  (char)0xDB, (char)0x5E, (char)0x74,  (char)0x56, (char)0xBF, (char)0x90,
  (char)0x1B, (char)0xC6, (char)0xE2,  (char)0xFE, (char)0x6A, (char)0x71,  (char)0x1D, (char)0xAC, (char)0x8F,  (char)0x9A, (char)0x46, (char)0x76,  (char)0xD3, (char)0x29, (char)0x17,
  (char)0xF3, (char)0x6F, (char)0x70,  (char)0xE2, (char)0x54, (char)0x62,  (char)0x33, (char)0x9A, (char)0x78,  (char)0x28, (char)0xC2, (char)0xC4,  (char)0x14, (char)0x84, (char)0x98,
  (char)0x5F, (char)0xD1, (char)0xC1,  (char)0xE6, (char)0x87, (char)0xC2,  (char)0x0B, (char)0x72, (char)0x5E,  (char)0xE6, (char)0x8C, (char)0x99,  (char)0x94, (char)0x2A, (char)0x4E,
  (char)0x00, (char)0x63, (char)0x3E,
  (char)(3 * 61),
  (char)0xF2, (char)0x08, (char)0x51,  (char)0xC8, (char)0x77, (char)0x8F,  (char)0x08, (char)0x4E, (char)0x26,  (char)0x4C, (char)0xE7, (char)0xC1,  (char)0x0F, (char)0x9A, (char)0xAF,
  (char)0x45, (char)0xB3, (char)0x9F,  (char)0x41, (char)0xC8, (char)0xC8,  (char)0xDC, (char)0x69, (char)0x8F,  (char)0x06, (char)0x7E, (char)0x5F,  (char)0xC6, (char)0x6D, (char)0x85,
  (char)0xD5, (char)0x3F, (char)0x3B,  (char)0x0A, (char)0x77, (char)0x68,  (char)0xF2, (char)0x73, (char)0x81,  (char)0x05, (char)0x76, (char)0x6C,  (char)0x29, (char)0xA7, (char)0x98,
  (char)0x26, (char)0xA6, (char)0xAC,  (char)0x24, (char)0xA5, (char)0xAB,  (char)0x19, (char)0x96, (char)0x9F,  (char)0x1F, (char)0xA7, (char)0x9F,  (char)0xDD, (char)0x6E, (char)0x8A,
  (char)0xE2, (char)0x60, (char)0x61,  (char)0xD2, (char)0x5F, (char)0x6B,  (char)0xD6, (char)0x58, (char)0x55,  (char)0xFF, (char)0x71, (char)0x5D,  (char)0x15, (char)0x99, (char)0x8C,
  (char)0x11, (char)0x92, (char)0x93,  (char)0x2F, (char)0xAC, (char)0x9D,  (char)0x2C, (char)0xAE, (char)0xAD,  (char)0x1C, (char)0x9F, (char)0xA3,  (char)0x07, (char)0x89, (char)0x8E,
  (char)0xEA, (char)0x7B, (char)0x7F,  (char)0xDE, (char)0x56, (char)0x59,  (char)0xE5, (char)0x64, (char)0x62,  (char)0xD7, (char)0x50, (char)0x59,  (char)0x01, (char)0x72, (char)0x65,
  (char)0x19, (char)0x98, (char)0x90,  (char)0x0C, (char)0x8B, (char)0x93,  (char)0x2D, (char)0xA0, (char)0x95,  (char)0x1B, (char)0xA5, (char)0xAF,  (char)0xFD, (char)0x85, (char)0x90,
  (char)0xFD, (char)0x7C, (char)0x7C,  (char)0xE6, (char)0x73, (char)0x7D,  (char)0xD7, (char)0x56, (char)0x5D,  (char)0xE6, (char)0x5F, (char)0x5B,  (char)0x00, (char)0x7E, (char)0x72,
  (char)0xEE, (char)0x77, (char)0x7E,  (char)0x0D, (char)0x80, (char)0x71,  (char)0x1E, (char)0xA0, (char)0x98,  (char)0x07, (char)0x8E, (char)0x94,  (char)0x0A, (char)0x83, (char)0x81,
  (char)0x0D, (char)0x91, (char)0x91,  (char)0xE8, (char)0x73, (char)0x81,  (char)0xF0, (char)0x69, (char)0x64,  (char)0xF4, (char)0x76, (char)0x74,  (char)0xE7, (char)0x6A, (char)0x6E,
  (char)0xF5, (char)0x6E, (char)0x68,  (char)0x03, (char)0x7F, (char)0x7A,  (char)0x08, (char)0x84, (char)0x82,  (char)0x09, (char)0x89, (char)0x8A,  (char)0x10, (char)0x8A, (char)0x88,
  (char)0x12, (char)0x94, (char)0x93,
  (char)(3 * 43),
  (char)0xF2, (char)0x08, (char)0x8D,  (char)0x02, (char)0x86, (char)0x8C,  (char)0xF8, (char)0x78, (char)0x7E,  (char)0xFA, (char)0x79, (char)0x78,  (char)0xED, (char)0x73, (char)0x78,
  (char)0xEE, (char)0x6A, (char)0x6B,  (char)0x01, (char)0x7D, (char)0x76,  (char)0xFE, (char)0x80, (char)0x81,  (char)0x04, (char)0x81, (char)0x7E,  (char)0x11, (char)0x8E, (char)0x89,
  (char)0x05, (char)0x8C, (char)0x91,  (char)0x03, (char)0x7F, (char)0x80,  (char)0x02, (char)0x84, (char)0x84,  (char)0xF9, (char)0x7B, (char)0x7C,  (char)0xF6, (char)0x77, (char)0x77,
  (char)0xFC, (char)0x79, (char)0x77,  (char)0xFE, (char)0x7E, (char)0x7D,  (char)0xFE, (char)0x7E, (char)0x7E,  (char)0x04, (char)0x80, (char)0x7E,  (char)0x08, (char)0x89, (char)0x87,
  (char)0xFF, (char)0x81, (char)0x84,  (char)0x03, (char)0x81, (char)0x80,  (char)0x02, (char)0x83, (char)0x84,  (char)0xFE, (char)0x7E, (char)0x81,  (char)0x00, (char)0x7E, (char)0x7D,
  (char)0xFF, (char)0x80, (char)0x80,  (char)0x00, (char)0x80, (char)0x7F,  (char)0x00, (char)0x80, (char)0x81,  (char)0x01, (char)0x80, (char)0x7F,  (char)0x01, (char)0x82, (char)0x82,
  (char)0xFF, (char)0x80, (char)0x80,  (char)0x02, (char)0x81, (char)0x80,  (char)0x00, (char)0x82, (char)0x82,  (char)0x00, (char)0x80, (char)0x80,  (char)0x02, (char)0x82, (char)0x81,
  (char)0x01, (char)0x81, (char)0x81,  (char)0xFF, (char)0x80, (char)0x80,  (char)0x02, (char)0x81, (char)0x80,  (char)0x00, (char)0x81, (char)0x82,  (char)0xFF, (char)0x7F, (char)0x7F,
  (char)0x00, (char)0x80, (char)0x80,  (char)0xFE, (char)0x7F, (char)0x80,  (char)0x00, (char)0x7F, (char)0x7E,
  (char)0
};

//tone=0xFFFFFF click, tone=0x000001 clack
int audio_chime_tone ( int tone )
{
  short int val = 0x7FF;

  if ( audio_write_y_reg ( AUDIO_Y_Sup_ExtonFL, &val, 1 ) ) {
    DBG();
    return -EIO;
  }

  if ( audio_write_y_reg ( AUDIO_Y_Sup_ExtonFR, &val, 1 ) ) {
    DBG();
    return -EIO;
  }

  if ( audio_write_x_reg ( AUDIO_X_WavTab_Control, &tone, 1 ) ) {
    DBG();
    return -EIO;
  }

  return 0;
}

static int audio_chime_set_data ( void )
{
  int cnt;
  unsigned char *data = ( unsigned char * ) audio_chime_data;

  while ( 1 ) {
    cnt = *data++;
    if ( cnt )
      if ( audio_send ( data, cnt ) ) {
        DBG ();
        return -EIO;
      } else break;
    data += cnt;
  }

  cnt = CLICK_CLACK_XRAM;
  if ( audio_write_x_reg ( AUDIO_X_WavTab_UseRamFlag, &cnt, 1 ) ) {
    DBG ();
    return -EIO;
  }

  cnt = 0;
  if ( audio_write_x_reg ( AUDIO_X_WavTab_Pointer, &cnt, 1 ) ) {
    DBG ();
    return -EIO;
  }

  cnt = AUDIO_REL_ADDR ( AUDIO_X_WavTab_Ram_BuffStart );
  if ( audio_write_x_reg ( AUDIO_X_WavTab_TicStartPntr, &cnt, 1 ) ) {
    DBG ();
    return -EIO;
  }

  cnt = AUDIO_REL_ADDR ( AUDIO_X_WavTab_Ram_BuffStart ) + XRAM_CLICK_SIZE - 1;
  if ( audio_write_x_reg ( AUDIO_X_WavTab_TicEndPntr, &cnt, 1 ) ) {
    DBG ();
    return -EIO;
  }

  cnt = AUDIO_REL_ADDR ( AUDIO_X_WavTab_Ram_BuffStart ) + XRAM_CLICK_SIZE;
  if ( audio_write_x_reg ( AUDIO_X_WavTab_TacStartPntr, &cnt, 1 ) ) {
    DBG ();
    return -EIO;
  }

  cnt = AUDIO_REL_ADDR ( AUDIO_X_WavTab_Ram_BuffStart ) + XRAM_CLICK_SIZE + XRAM_CLACK_SIZE - 1;
  if ( audio_write_x_reg ( AUDIO_X_WavTab_TacEndPntr, &cnt, 1 ) ) {
    DBG ();
    return -EIO;
  }

  return 0;
}

int audio_chime_init ( void )
{
  int val;

  if ( audio_init() ) {
    DBG();
    return -ENODEV;
  }

  val = AUDIO_REL_ADDR ( AUDIO_X_FrontOutL );
  if ( audio_write_x_reg ( AUDIO_X_FDACpntr, &val, 1 ) ) {
    DBG ();
    return -EIO;
  }

  val = AUDIO_REL_ADDR ( AUDIO_X_RearOutL );
  if ( audio_write_x_reg ( AUDIO_X_RDACpntr, &val, 1 ) ) {
    DBG ();
    return -EIO;
  }

  val = CLICK_CLACK_XRAM;

  switch ( val ) {
  case CLICK_CLACK_ROM:
    if ( audio_write_x_reg ( AUDIO_X_WavTab_UseRamFlag, &val, 1 ) ) {
      DBG ();
      return -EIO;
    }
    break;
  case CLICK_CLACK_XRAM:
    if ( audio_chime_set_data() ) {
      DBG ();
      return -EIO;
    }
    break;
  default:
    DBG ();
    return -EINVAL;
  }

  val = 0;
  if ( audio_write_x_reg ( AUDIO_X_WavTab_UseIOFlag, &val, 1 ) ) {
    DBG ();
    return -EIO;
  }

  return 0;
}

int audio_chime_generate ( audio_chime_type* chime, int loop )
{
  if ( chime == NULL ) {
    DBG ();
    return -EINVAL;
  }

  short int y[2] = {0}, tmp_y = 0;
  int cnt = 0, tmp_x = 0;
  double tmp_z;
  audio_chime_slot* src = chime->slots;
  audio_chime_addr* dst = ( audio_chime_addr* ) audio_chime_addrs;

  if ( audio_write_y_reg ( AUDIO_Y_Tsq_Copied, &tmp_y, 1 ) ) {
    DBG();
    return -EIO;
  }

  while ( cnt++ < SLOT_CNT ) {
    //Slot Freq
    tmp_z = 2 * src->freq / AUDIO_SAMPLE_FREQ;
    audio_data_conv ( audio_calc_x ( src->wave_type == CHIME_SINE_WAVE ? cos ( tmp_z * M_PI ) : tmp_z ), y );
    if ( audio_write_y_reg ( dst->freq_high_addr, y, 1 ) ) {
      DBG();
      return -EIO;
    }
    if ( audio_write_y_reg ( dst->freq_low_addr, y + 1, 1 ) ) {
      DBG();
      return -EIO;
    }

    //Slot Wave Type
    tmp_y = src->wave_type == CHIME_SINE_WAVE ? CHIME_SINE_WAVE : CHIME_TRIANGULAR_WAVE;
    if ( audio_write_y_reg ( dst->wave_type_addr, &tmp_y, 1 ) ) {
      DBG();
      return -EIO;
    }

    //Clipping Factor
    //tmp_y = audio_calc_y ( ( pow ( 10.0, arg / 20.0 ) ) / 16 );
    tmp_y = 128;
    if ( audio_write_y_reg ( dst->clip_factor_addr, &tmp_y, 1 ) ) {
      DBG();
      return -EIO;
    }

    //Slot Duration
    tmp_x = audio_calc_x ( AUDIO_SAMPLE_FREQ / 8000 * src->dur / 0x1000000 );
    if ( audio_write_x_reg ( dst->dur_addr, &tmp_x, 1 ) ) {
      DBG();
      return -EIO;
    }

    //Slot Volume
    tmp_y = audio_calc_y ( -pow ( 10.0,  src->vol / 20.0 ) );
    if ( audio_write_y_reg ( dst->vol_addr, &tmp_y, 1 ) ) {
      DBG();
      return -EIO;
    }

    src++;
    dst++;
  };

  //Master Volume
  tmp_y = audio_calc_y ( pow ( 10.0, chime->vol / 20.0 ) );
  if ( audio_write_y_reg ( AUDIO_Y_Tsq_ChimeVol_Bk, &tmp_y, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Cross-Fader Timing
  //tmp_x = audio_calc_x ( 4000 / AUDIO_SAMPLE_FREQ / arg );
  tmp_x = audio_calc_x ( 4000 / AUDIO_SAMPLE_FREQ );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_CrosFStp_Bk, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Attack Timing
  tmp_x = audio_calc_x ( 4000 / AUDIO_SAMPLE_FREQ / chime->attack_time );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_ArLinStpA_Bk, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Release Type
  tmp_y = chime->release_type == CHIME_EXPONENTIAL_RELEASE ? 0x7FF : 0;
  if ( audio_write_y_reg ( AUDIO_Y_Tsq_ArType_Bk, &tmp_y, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Release Timing
  tmp_z = 4000 / AUDIO_SAMPLE_FREQ / chime->release_time;
  tmp_x = audio_calc_x ( chime->release_type == CHIME_EXPONENTIAL_RELEASE ? exp ( -tmp_z ) : tmp_z );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_ArRelCoef_Bk, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Sequencer Timing
  tmp_x = audio_calc_x ( AUDIO_SAMPLE_FREQ / 8000 * chime->signal_on_time / 0x1000000 );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_TA_Bk, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  tmp_x = audio_calc_x ( AUDIO_SAMPLE_FREQ / 8000 * chime->signal_off_time / 0x1000000 );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_TB_Bk, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Repetitions
  tmp_x = loop == 0 ? 0x800000 : loop + 1;
  if ( audio_write_x_reg ( AUDIO_X_Tsq_repeate_Bk, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Change Threshold
  tmp_x = audio_calc_x ( pow ( 10.0, chime->change_threshold / 20.0 ) );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_ChangeThreshold, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Decay Threshold
  if ( chime->release_type == CHIME_EXPONENTIAL_RELEASE ) {
    tmp_z = -1.0 * chime->exponential_decay_threshold  / chime->tau;
    tmp_z = exp ( ( float ) tmp_z );
  } else {
    tmp_z = chime->linear_decay_threshold / 20.0;
    tmp_z = pow ( 10.0, tmp_z );
  }
  tmp_x = audio_calc_x ( tmp_z );
  if ( audio_write_x_reg ( AUDIO_X_Tsq_DecayThreshold, &tmp_x, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Turn Chime Generator ON
  tmp_y = 0x7FF;
  if ( audio_write_y_reg ( AUDIO_Y_Tsq_SeqStat_Bk, &tmp_y, 1 ) ) {
    DBG();
    return -EIO;
  }

  //Activate the chime.
  tmp_y = 0x0800;
  if ( audio_write_y_reg ( AUDIO_Y_Tsq_Copied, &tmp_y, 1 ) ) {
    DBG();
    return -EIO;
  }

  return 0;
}

#ifdef SIMULATION
void main ( void )
{
  void *tst;

  audio_read_y_reg ( 0xff, NULL, 1 );
  audio_mute_ctl ( ( audio_ctl_parm* ) tst, 0, 0 );
  audio_scaler_ctl ( ( audio_ctl_parm* ) tst, 0 );
  audio_filter_ctl ( ( audio_ctl_parm* ) tst, ( audio_filter_parm* ) tst );
  audio_chime_init();
  audio_chime_tone ( 1 );
  audio_chime_generate ( ( audio_chime_type * ) tst, 0 );
}
#endif

