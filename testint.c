#include <stdint.h>
#include <iostream>
int main (){
  int16_t a = 0x000b;
  uint16_t b = 6;
  std::cout << (a | 0xf000) << std::endl;
  return 0;
}
