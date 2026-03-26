#include "Application.hpp"

#include <iostream>
#include <stdexcept>

int main() 
{
    try
    {
        Application app("Vulkan Test", 960, 540);
        app.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        EXIT_FAILURE;
	}
    EXIT_SUCCESS;
}