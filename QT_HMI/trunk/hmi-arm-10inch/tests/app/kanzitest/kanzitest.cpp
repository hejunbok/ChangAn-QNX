#include <iostream>
#include <kanzi/kanzi.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>


void onSetUI(const KANZI::idxUI idx)
{
    std::cout<<"On set UI"<<(int)idx<<std::endl;
}

void onFL(void)
{
    std::cout<<"onFL"<<std::endl;
}

void onFR(void)
{
    std::cout<<"onFR"<<std::endl;
}

void onRL(void)
{
    std::cout<<"onRL"<<std::endl;
}

void onRR(void)
{
    std::cout<<"onRR"<<std::endl;
}

int main(int argc, char **argv)
{
    if(argc == 2)
    {
     //   std::cout<<"set ui "<<argv[1]<<std::endl;
        if(strcmp("1", argv[1]) == 0 )
        {
            std::cout<<"set ui navi"<<std::endl;
//            KANZI::set_ui(KANZI::NAVI);
        }
        if(strcmp("2", argv[1]) == 0 )
        {
            std::cout<<"set ui media"<<std::endl;
//            KANZI::set_ui(KANZI::MEDIA);
        }
        if(strcmp("3", argv[1]) == 0 )
        {
            std::cout<<"set ui setting"<<std::endl;
//            KANZI::set_ui(KANZI::SETTING);
        }
        if(strcmp("FL", argv[1]) == 0 )
        {
            std::cout<<"test FL"<<std::endl;
//            KANZI::toggleFL();
        }
        if(strcmp("FR", argv[1]) == 0 )
        {
            std::cout<<"test FR"<<std::endl;
//            KANZI::toggleFR();
        }
        if(strcmp("RL", argv[1]) == 0 )
        {
            std::cout<<"test RL"<<std::endl;
//            KANZI::toggleRL();
        }
        if(strcmp("RR", argv[1]) == 0 )
        {
            std::cout<<"test RR"<<std::endl;
//            KANZI::toggleRR();
        }
        if(strcmp("0", argv[1]) == 0 )
        {
            KANZI::Kanzi_CB cbs = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            cbs.pSetUI = onSetUI;
            cbs.pSettingToggleFL = onFL;
            cbs.pSettingToggleFR = onFR;
            cbs.pSettingToggleRL = onRL;
            cbs.pSettingToggleRR = onRR;

            std::cout<<"start IPC Loop"<<std::endl;
            KANZI::runIPCLoop(&cbs);

        }
    }

    return 0;
}
