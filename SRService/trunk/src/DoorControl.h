#ifndef DOORCONTROL_H
#define DOORCONTROL_H

#include "IControl.h"
#include <string>
#include <vector>

extern void toggleDoor(const char* door, int state);
 
class Door
{
   std::string m_name;
   std::vector<std::string> m_keys;

public:
   Door(std::string name)
      : m_name(name)
   {
   };

   ~Door(){}

   void addKey(std::string key)
   {
      m_keys.push_back(key);
   }

   bool match(std::string command)
   {
      bool ret = true;
      for (int i = 0; i < m_keys.size(); i++)
      {
	 if (command.find(m_keys[i]) == std::string::npos)
	 {
	    ret = false;
	    break;
	 }
      }
      
      return ret;
   }

   void operation(int state)
   {
      toggleDoor(m_name.c_str(), state);
   };
};

class DoorControl : public IControl
{
   std::vector<Door*> m_doors;

   public:
   DoorControl();
   ~DoorControl();

   virtual bool match(std::string command);
};

#endif
