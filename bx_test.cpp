
#include "bx.h"
#include "bx.cpp"

#include <stdio.h>

entry_point int main(int argc, char** argv) {
     rng rand = {};
     seed(&rand, 1234);
     
     repeat(32) {
          f64 i = range_f64(&rand, -5.0f, 5.0f);
          printf("%lf\n", i);
     }
     
     return 0;
}