#include "version.h"
#include <iostream>

int main()
{
  std::cout << "Software Version: " << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "."
            << PROJECT_VERSION_PATCH << std::endl;

  std::cout << "Version String: " << PROJECT_VERSION_STRING << std::endl;
  return 0;
}
