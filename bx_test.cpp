
#include "bx.h"
#include "bx.cpp"

#include <stdio.h>

int main(int argc, char** argv) {
     
     printf("compiler: %s\n", COMPILER_NAME);
     printf("platform: %s\n", PLATFORM_NAME);
     printf("architecture: %s\n", ARCHITECTURE_NAME);
     
     return 0;
}