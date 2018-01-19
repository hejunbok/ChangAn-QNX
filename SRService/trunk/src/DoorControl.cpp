#include "DoorControl.h"

DoorControl::DoorControl()
{
   Door* fl = new Door("fl");
   fl->addKey("前");
   fl->addKey("左");

   Door* fr = new Door("fr");
   fr->addKey("前");
   fr->addKey("右");

   Door* rl = new Door("rl");
   rl->addKey("后");
   rl->addKey("左");

   Door* rr = new Door("rr");
   rr->addKey("后");
   rr->addKey("右");

   Door* bd = new Door("backdoor");
   bd->addKey("后");
   bd->addKey("箱");

   m_doors.push_back(fl);
   m_doors.push_back(fr);
   m_doors.push_back(rl);
   m_doors.push_back(rr);
   m_doors.push_back(bd);
}

DoorControl::~DoorControl()
{
   for (int i = 0; i < m_doors.size(); i++)
   {
      delete m_doors[i];
   }
}

bool DoorControl::match(std::string command)
{
   bool ret = false;
   int state = -1;

   //if (command.find("门") != std::string::npos || command.find("窗") != std::string::npos)
   //{
   if (command.find("开") != std::string::npos || command.find("open") != std::string::npos)
   {
      state = 1;
   }
   else if (command.find("关") != std::string::npos || command.find("close") != std::string::npos)
   {
      state = 0;
   }
   //}

   if (state != -1)
   {
      for (int i = 0; i < m_doors.size(); i++)
      {
	 if (m_doors[i]->match(command))
	 {
	    m_doors[i]->operation(state);
	    ret = true;
	    break;
	 }
      }

      if (!ret & command.find("门") != std::string::npos)
      {
	 for (int i = 0; i < m_doors.size(); i++)
	 {
	    m_doors[i]->operation(state);
	 }

	 ret = true;
      }
   }

   return ret;
}
