#include "Controller.h"

#include <iostream>

Controller::Controller(QObject *parent) :
    QObject(parent)
{
}

void Controller::show()
{
    std::cout << "show\n";
    std::cout.flush();
}

void Controller::hide()
{
    std::cout << "hide\n";
    std::cout.flush();
}
