#include "version.h"
#include <iostream>

int main()
{
  std::cout << "Project Description: ";
  std::cout << PROJECT_DESCRIPTION << std::endl;
  std::cout << "Project Software Version: ";
  std::cout << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "." << PROJECT_VERSION_PATCH << std::endl;
  std::cout << "Project Version String: ";
  std::cout << PROJECT_VERSION_STRING << std::endl;

  return 0;
}
