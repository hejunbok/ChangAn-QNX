#include <stdio.h>
#include "audio_hero_api.h"

#define MSG(x...) printf(x)

int main ( void )
{
  if ( audio_hero_init() == 0 )
    MSG ( "*****audio_hero_init() success.\n" );
  if ( audio_hero_select_source ( AUDIO_HOST_IIS0, AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) == 0 )
    MSG ( "*****audio_hero_select_source() success.\n" );
  if ( audio_hero_set_volume ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL, -10 ) == 0 ) {
    MSG ( "*****audio_hero_set_volume() success.\n" );
  }
  if ( audio_hero_select_source ( AUDIO_AIN2, AUDIO_SEC_PHYSICAL_CHNL ) == 0 )
	  MSG ( "*****audio_hero_select_source() success.\n" );

  return 0;
}
