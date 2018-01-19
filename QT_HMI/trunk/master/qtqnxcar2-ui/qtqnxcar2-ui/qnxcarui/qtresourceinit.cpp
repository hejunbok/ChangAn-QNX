#include "qtresourceinit.h"

extern int qInitResources_qtqnxcar2_ui();

void QnxCarUi::initQtResources()
{
    static bool initialized = false;
    if (!initialized) {
        qInitResources_qtqnxcar2_ui();
        initialized = true;
    }
}
