#include "LightControl.h"

extern void toggleDoor(const char* key, int value);

LightControl::Light::Light(std::string name)
: m_name(name)
{
}

LightControl::Light::~Light()
{
}

void LightControl::Light::addKey(std::string key)
{
   m_keys.push_back(key);
}

bool LightControl::Light::match(std::string command)
{
   bool ret = true;

   for(auto key : m_keys)
   {
      if (command.find(key) == std::string::npos)
      {
	 ret = false;
	 break;
      }
   }

   return ret;
}

void LightControl::Light::operation(int state)
{
   toggleDoor(m_name.c_str(), state);
}

LightControl::LightControl()
{
   Light *front = new Light("frontlights");
   front->addKey("灯");

   m_lights.push_back(front);
}

LightControl::~LightControl()
{
   for (auto light : m_lights)
   {
      delete light;
   }
}

bool LightControl::match(std::string command)
{
   bool ret = false;
   int state = -1;

   if (command.find("开") != std::string::npos || command.find("open") != std::string::npos)
   {
      state = 1;
   }
   else if (command.find("关") != std::string::npos || command.find("close") != std::string::npos)
   {
      state = 0;
   }
   
   if (state != -1)
   {
     for (auto light : m_lights)
     {
	if (light->match(command))
	{
	   light->operation(state);
	   ret = true;
	   break;
	}
     }
   }

   return ret;
}
