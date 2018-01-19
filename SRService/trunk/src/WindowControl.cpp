#include "WindowControl.h"

extern void toggleDoor(const char* key, int value);

WindowControl::Window::Window(std::string name)
: m_name(name)
{
}

WindowControl::Window::~Window()
{
}

void WindowControl::Window::addKey(std::string key)
{
   m_keys.push_back(key);
}

bool WindowControl::Window::match(std::string command)
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

void WindowControl::Window::operation(int state)
{
   toggleDoor(m_name.c_str(), state);
}

WindowControl::WindowControl()
{
   Window *fl = new Window("window_fl");
   fl->addKey("前");
   fl->addKey("左");

   Window* fr = new Window("window_fr");
   fr->addKey("前");
   fr->addKey("右");

   Window* rl = new Window("window_rl");
   rl->addKey("后");
   rl->addKey("左");

   Window* rr = new Window("window_rr");
   rr->addKey("后");
   rr->addKey("右");

   m_windows.push_back(fl);
   m_windows.push_back(fr);
   m_windows.push_back(rl);
   m_windows.push_back(rr);
}

WindowControl::~WindowControl()
{
   for (auto window : m_windows)
   {
      delete window;
   }
}

bool WindowControl::match(std::string command)
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
   
   if (state != -1 && command.find("窗") != std::string::npos)
   {
     for (auto window : m_windows)
     {
	window->operation(state);
     }

     ret = true;
   }

   return ret;
}
