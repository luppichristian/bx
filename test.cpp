
#include "basic.h"
#include "basic.cpp"

#include <stdio.h>

internal void test_rng(void) {
      rng rn = {};
      
      u32 probe_count = 10000;
      
      seed(&rn, 1234);
      
      printf("0%%\n");
      
      for(s32 i = 0; i < probe_count; ++i) {
            u32 x = next_u32(&rn);
            u64 y = next_u64(&rn);
            s32 z = next_s32(&rn);
            s64 w = next_s64(&rn);
            f32 a = next_f32(&rn);
            f64 b = next_f64(&rn);
            
            debug_step();
      }
      
      printf("33,33%%\n");
      
      for(s32 i = 0; i < probe_count; ++i) {
            u32 x = range_u32(&rn, 40, 50);
            assert(in_range(x, 40, 50));
            
            u64 y = range_u64(&rn, 5000, 10000);
            assert(in_range(y, 5000, 10000));
            
            s32 z = range_s32(&rn, -100, 50);
            assert(in_range(z, -100, 50));
            
            s64 w = range_s64(&rn, -40, 10);
            assert(in_range(w, -40, 10));
            
            f32 a = range_f32(&rn, 40, 60);
            assert(in_range(a, 40, 60));
            
            f64 b = range_f64(&rn, 40.04, 50.04);
            assert(in_range(b, 40.04, 50.04));
            
            debug_step();
      }
      
      printf("66,66%%\n");
      
      for(s32 i = 0; i < probe_count; ++i) {
            f32 a = unilateral_f32(&rn);
            assert(is_unilateral(a));
            
            f32 b = unilateral_f64(&rn);
            assert(is_unilateral(b));
            
            f32 c = bilateral_f32(&rn);
            assert(is_bilateral(c));
            
            f32 d = unilateral_f64(&rn);
            assert(is_bilateral(d));
      }
      
      printf("100,0%%\n");
}

entry_point int main(int argc, char** argv) {
      test_rng();
      
      f32 c0 = cos(0.0f);
      f32 c1 = cos(1.0f);
      
      return 0;
}