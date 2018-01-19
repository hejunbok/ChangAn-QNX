#include "FmHardwareInterface.h"
#include "tuner.h"

#include <stdio.h>

FmHardwareInterface* FmHardwareInterface::create()
{
   static FmHardwareInterface *instance = NULL;

   if (!instance)
   {
      instance = new tuner();
   }

   return instance;
}
