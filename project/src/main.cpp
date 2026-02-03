#include "chrono/physics/ChSystemSMC.h"
#include <iostream>

int main() {
  std::cout << "Chrono system initialized successfully!" << std::endl;

  chrono::ChSystemSMC system;
  std::cout << "ChSystem created: " << &system << std::endl;

  return 0;
}
