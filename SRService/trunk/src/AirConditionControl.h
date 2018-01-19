#ifndef AIRCONDITIONCONTROL_H
#define AIRCONDITIONCONTROL_H

#include "IControl.h"
#include <string>

class AirConditionControl : public IControl
{
   void operation(int state);
public:
   AirConditionControl();
   ~AirConditionControl();
   virtual bool match(std::string command);
};

#endif
