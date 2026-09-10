#include "Controller.h"
#include <iostream>

int main()
{
    try {
        Controller controller;
        controller.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
}
