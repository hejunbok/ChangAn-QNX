#include <stdio.h>
#include <unistd.h>
#include "audio_hero_api.h"

int main ( int argc, char **argv )
{
  int c;
  unsigned char data[3] = {0};

  data[0] = 0x20;

  if ( ( c = getopt ( argc, argv, "ir" ) ) != -1 ) {
    switch ( c ) {
    case 'i':
      printf ( "Select HOST IIS as input.\n" );
      data[1] = 0x13;
      if ( audio_send ( data, 2 ) == 0 ) {
        printf ( "*****audio_hero_select_source() success.\n" );
      }
      break;
    case 'r':
      printf ( "Select Radio as input.\n" );
      data[1] = 0x00;
      if ( audio_send ( data, 2 ) == 0 ) {
        printf ( "*****audio_hero_select_source() success.\n" );
      }
      break;
    default:
      printf ( "Select default input.\n" );
      break;
    }
  }

  return 0;
}

