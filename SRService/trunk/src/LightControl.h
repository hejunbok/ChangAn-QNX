#ifndef LIGHTCONTROL_H
#define LIGHTCONTROL_H

#include "IControl.h"
#include <string>
#include <vector>

class LightControl : public IControl
{
public:
   LightControl();
   virtual ~LightControl();

private:
class Light
{
   std::string m_name;
   std::vector<std::string> m_keys;

public:
   Light(std::string name);
   virtual ~Light();

   void addKey(std::string key);
   bool match(std::string command);

   void operation(int state);
};

   std::vector<Light*> m_lights;

public:
   virtual bool match(std::string command);
};

#endif
