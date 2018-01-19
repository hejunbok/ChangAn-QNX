#include "AirConditionControl.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>

AirConditionControl::AirConditionControl()
{
}

AirConditionControl::~AirConditionControl()
{
}

void AirConditionControl::operation(int state)
{
   int fd = 0;
   int length = 0;
   char buf[32] = {0};

   
   sprintf(buf, "airConditioning_all:b:%s\n", state ? "true" : "false");
   
   length = strlen(buf);

   fd = open("/pps/qnxcar/hvac", O_RDWR | O_CREAT, 0666);
   if (fd > 0)
   {
      if (write(fd, buf, length) == length)
      {
	 std::cout << "AirConditionControl::operation OK" << std::endl;
      }

      close(fd);
   }
   else
   {
      std::cout << "AirConditionControl::operation open error:" << strerror(errno) << std::endl;
   }

   sprintf(buf, "%s:n:%d\n"
	 "%s:n:%d\n", "cmd", 0, "onoff", state);
   
   length = strlen(buf);

   fd = open("/pps/services/air_control", O_RDWR | O_CREAT, 0666);
   if (fd > 0)
   {
      if (write(fd, buf, length) == length)
      {
	 std::cout << "AirConditionControl::operation OK" << std::endl;
      }

      close(fd);
   }
   else
   {
      std::cout << "AirConditionControl::operation open error:" << strerror(errno) << std::endl;
   }
}

bool AirConditionControl::match(std::string command)
{
   bool ret = false;
   int state = -1;

   if (command.find("air_control") != std::string::npos || command.find("空调") != std::string::npos)
   {
      if (command.find("open") != std::string::npos || command.find("开") != std::string::npos)
      {
	 state = 1;
      }
      else if (command.find("close") != std::string::npos || command.find("关") != std::string::npos)
      {
	 state = 0;
      }
   }

   if (state != -1)
   {
      operation(state);
      ret = true;
   }

   std::cout << "AirConditionControl::match state = " << state << std::endl;

   return ret;
}
