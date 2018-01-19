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
}

void onFL(void)
{
}

void onFR(void)
{
}

void onRL(void)
{
}

void onRR(void)
{
}

int main(int argc, char **argv)
{
    if(argc == 2)
    {
        if(strcmp("1", argv[1]) == 0 )
        {
//            KANZI::set_ui(KANZI::NAVI);
        }
        if(strcmp("2", argv[1]) == 0 )
        {
//            KANZI::set_ui(KANZI::MEDIA);
        }
        if(strcmp("3", argv[1]) == 0 )
        {
//            KANZI::set_ui(KANZI::SETTING);
        }
        if(strcmp("FL", argv[1]) == 0 )
        {
//            KANZI::toggleFL();
        }
        if(strcmp("FR", argv[1]) == 0 )
        {
//            KANZI::toggleFR();
        }
        if(strcmp("RL", argv[1]) == 0 )
        {
//            KANZI::toggleRL();
        }
        if(strcmp("RR", argv[1]) == 0 )
        {
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

            KANZI::runIPCLoop(&cbs);

        }
    }

    return 0;
}
