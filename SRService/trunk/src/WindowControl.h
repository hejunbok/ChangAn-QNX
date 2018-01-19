#ifndef WINDOWCONTROL_H
#define WINDOWCONTROL_H

#include "IControl.h"
#include <string>
#include <vector>

class WindowControl : public IControl
{
public:
   WindowControl();
   virtual ~WindowControl();

private:
class Window
{
   std::string m_name;
   std::vector<std::string> m_keys;

public:
   Window(std::string name);
   virtual ~Window();

   void addKey(std::string key);
   bool match(std::string command);

   void operation(int state);
};

   std::vector<Window*> m_windows;

public:
   virtual bool match(std::string command);
};

#endif
