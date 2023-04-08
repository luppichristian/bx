
#if !defined(BX)
#error Missing header!
#endif

void* copy(void* dst, void* src, sz size) {
     for(sz i = 0; i < size; ++i) ((u8*)dst)[i] = ((u8*)src)[i];
     return dst;
}

void* set8(void* dst, u8 byte, sz count) {
     for(sz i = 0; i < count; ++i) ((u8*)dst)[i] = byte;
     return dst;
}

void* set16(void* dst, u16 word, sz count) {
     for(sz i = 0; i < count; ++i) ((u16*)dst)[i] = word;
     return dst;
}

void* set32(void* dst, u32 dword, sz count) {
     for(sz i = 0; i < count; ++i) ((u32*)dst)[i] = dword;
     return dst;
}

void* set64(void* dst, u64 qword, sz count) {
     for(sz i = 0; i < count; ++i) ((u64*)dst)[i] = qword;
     return dst;
}

void* zero(void* dst, sz size) {
     set8(dst, 0, size);
     return dst;
}

void* move(void* dst, void* src, sz size) {
     if(dst < src) {
          for(sz i = 0; i < size; ++i) {
               ((u8*)dst)[i] = ((u8*)src)[i];
          }
     } else if(dst > src) {
          for(sz i = 0; i < size; ++i) {
               ((u8*)dst)[size - i] = ((u8*)src)[size - i];
          }
     }
     
     return dst;
}

sz compress(void* dst, void* src, sz size) {
     u8 literal_count = 0;
     u8 literals[U8_MAX];
     u8* in = (u8*)src;
     u8* in_max = in + size;
     u8* out = (u8*)dst;
     u8* out_max = out + size;
     bool dont_compress = false;
     if(!dont_compress) {
          while(in <= in_max) {
               sz best_run = 0;
               sz best_distance = 0;
               for(u8* window_start = in - (min(in - ((u8*)src), U8_MAX)); window_start < in; ++window_start) {
                    sz run = 0;
                    u8* scan_at = in;
                    u8* window_at = window_start;
                    while((window_at < in) && (scan_at != in_max) && (*scan_at++ == *window_at++)) {
                         run++;
                    }
                    
                    if(run > best_run) {
                         best_run = run;
                         best_distance = (sz)(in - window_start);
                    }
               }
               
               bool output_run = (literal_count) ? (best_run > 4) : (best_run > 2);
               if((in == in_max) || output_run || (literal_count == U8_MAX)) {
                    if(literal_count) {
                         if((out + literal_count + 2) > out_max) {
                              dont_compress = true;
                              break;
                         } else {
                              *out++ = literal_count;
                              *out++ = 0;
                              for(sz index = 0; index < literal_count; ++index) *out++ = literals[index];
                              literal_count = 0;
                         }
                    }
                    
                    if(output_run) {
                         assert(best_run <= U8_MAX);
                         assert(best_distance <= U8_MAX);
                         if((out + 2) > out_max) {
                              dont_compress = true;
                              break;
                         } else {
                              *out++ = (u8)best_run;
                              *out++ = (u8)best_distance;
                              in += best_run;
                              assert(in <= in_max);
                         }
                    }
                    
                    if(in == in_max) break;
               } else {
                    assert(literal_count < U8_MAX);
                    literals[literal_count++] = *in++;
                    assert(in <= in_max);
               }
          }
     }
     
     sz out_size = 0;
     if(!dont_compress) {
          assert(in == in_max);
          assert(!literal_count);
          out_size = (sz)(out - ((u8*)dst));
          assert(out_size <= size);
          if(out_size == size) {
               dont_compress = true;
          }
     }
     
     if(dont_compress) {
          copy(dst, src, size);
          out_size = size;
     }
     
     return out_size;
}

void decompress(void* dst, void* src, sz size, sz decompressed_size) {
     if(decompressed_size != size) {
          u8* out = (u8*)dst;
          u8* out_max = out + decompressed_size;
          u8* in = (u8*)src;
          u8* in_max = in + size;
          while(in < in_max) {
               u8 Count = *in++;
               u8 Distance = *in++;
               if(Distance) {
                    u8 *Source = (out - Distance);
                    while(Count--) *out++ = *Source++;
               } else {
                    while(Count--) *out++ = *in++;
               }
          }
          
          assert(in == in_max);
          assert(out == out_max);
     } else {
          copy(dst, src, size);
     }
}