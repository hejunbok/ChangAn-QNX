#ifndef ICONTROL_H
#define ICONTROL_H

#include <string>

class IControl
{
public:
   virtual bool match(std::string command) = 0;
};

#endif
