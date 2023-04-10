
// Copyright 2023 Christian Luppi
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to use the Software for learning purposes only. 
// Any commercial use, reproduction, distribution, modification, or creation of derivative works from this Software, 
// or any part thereof, is strictly prohibited without the explicit written permission of the copyright owner.
// THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#if !defined(BX)
#error Missing header!
#endif

u32 f32_to_u32(f32 x) {
     return *((u32*)(&x));
}

u64 f64_to_u64(f64 x) {
     return *((u64*)(&x));
}

f32 f32_from_u32(u32 x) {
     return *((f32*)(&x));
}

f64 f64_from_u64(u64 x) {
     return *((f64*)(&x));
}

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

void* align(void* ptr, up alignment) {
     assert(ptr && is_pow2(alignment));
     return uint_to_ptr(align_pow2(uint_from_ptr(ptr), alignment));
}

void* align(void* ptr, up padding, up alignment) {
     u8* forward = (u8*)ptr + padding;
     return align(forward, alignment);
}

bool compare(void* a, void* b, sz size) {
     for(sz i = 0; i < size; ++i) {
          if(((u8*)a)[i] != ((u8*)b)[i]) {
               return false;
          }
     }
     
     return true;
}

sz compress_lz(void* dst, void* src, sz size) {
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

sz compress_rle(void* dst, void* src, sz size) {
     u8* out = (u8*)dst;
     u8* out_max = out + size;
     sz literal_count = 0;
     u8 literals[U8_MAX];
     u8* in = (u8*)src;
     u8* in_max = in + size;
     bool dont_compress = false;
     while(in <= in_max) {
          u8 starting_value = (in == in_max) ? 0 : in[0];
          sz run = 0;
          while((run < (sz)(in_max - in)) && (run < U8_MAX) && (in[run] == starting_value)) {
               ++run;
          }
          
          if((in == in_max) || (run > 1) || (literal_count == U8_MAX)) {
               u8 literal_count8 = (u8)literal_count;
               assert(literal_count8 == literal_count);
               
               if((out + 1 + literal_count) > out_max) {
                    dont_compress = true;
                    break;
               }
               
               *out++ = literal_count8;
               
               for(sz Literalindex = 0; Literalindex < literal_count; ++Literalindex) {
                    *out++ = literals[Literalindex];
               }
               
               literal_count = 0;
               
               u8 run8 = (u8)run;
               assert(run8 == run);
               
               if((out + 2) > out_max) {
                    dont_compress = true;
                    break;
               }
               
               *out++ = run8;
               *out++ = starting_value;
               in += run;
               if(in == in_max) break;
          } else {
               literals[literal_count++] = starting_value;
               ++in;
          }
     }
     
     sz out_size = 0;
     if(!dont_compress) {
          assert(in == in_max);
          assert(!literal_count);
          out_size = (sz)(out - (u8*)dst);
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

void decompress_lz(void* dst, void* src, sz size, sz decompressed_size) {
     if(decompressed_size != size) {
          u8* out = (u8*)dst;
          u8* out_max = out + decompressed_size;
          u8* in = (u8*)src;
          u8* in_max = in + size;
          while(in < in_max) {
               u8 count = *in++;
               u8 distance = *in++;
               if(distance) {
                    u8 *Source = (out - distance);
                    while(count--) *out++ = *Source++;
               } else {
                    while(count--) *out++ = *in++;
               }
          }
          
          assert(in == in_max);
          assert(out == out_max);
     } else {
          copy(dst, src, size);
     }
}

void decompress_rle(void* dst, void* src, sz size, sz decompressed_size) {
     if(decompressed_size != size) {
          u8* out = (u8*)dst;
          u8* in = (u8*)src;
          u8* in_max = in + size;
          while(in < in_max) {
               u8 literal_count = *in++;
               while(literal_count--) {
                    *out++ = *in++;
               }
               
               u8 rep_count = *in++;
               u8 rep_value = *in++;
               while(rep_count--) {
                    *out++ = rep_value;
               }
          }
          
          assert(in == in_max);
     } else {
          copy(dst, src, size);
     }
}

b8x are_sorted(sort_entry* entries, u32 count) {
     bool sorted = true;
     if(count > 1) {
          for(u32 i = 0; i < (count - 1); i++) {
               sort_entry* e0 = entries + i;
               sort_entry* e1 = e0 + 1;
               if(e0->key > e1->key) {
                    sorted = false;
                    break;
               }
          }
     }
     
     return sorted;
}

void sort_bubble(sort_entry* entries, u32 count) {
     if(count > 1) {
          for(u32 outer = 0; outer < count; outer++) {
               bool Sorted = true;
               for(u32 inner = 0; inner < (count - 1); inner++) {
                    sort_entry* e0 = entries + inner;
                    sort_entry* e1 = e0 + 1;
                    if(e0->key > e1->key) {
                         rswap(e0, e1);
                         Sorted = false;
                    }
               }
               
               if(Sorted) {
                    break;
               }
          }
     }
}

internal void sort_quick_internal(sort_entry* entries, u32 first_entry, u32 last_entry) {
     if(first_entry < last_entry) {
          u32 pivot = first_entry;
          u32 i = first_entry;
          u32 j = last_entry;
          
          while(i < j) {
               while(entries[i].key <= entries[pivot].key && i < last_entry) i++;
               while(entries[j].key > entries[pivot].key) j--;
               if(i < j) rswap(&entries[i], &entries[j]);
          }
          
          rswap(&entries[pivot], &entries[j]);
          sort_quick_internal(entries, first_entry, j-1);
          sort_quick_internal(entries, j+1, last_entry);
     }
}

void sort_quick(sort_entry* entries, u32 count) {
     sort_quick_internal(entries, 0, count-1);
}

void sort_radix(sort_entry* entries, u32 count) {
     if(count > 1) {
          sort_entry* temp = 0; // TODO: allocate from scratch memory.
          sort_entry* src = entries;
          sort_entry* dst = temp;
          for(u32 byte_index = 0; byte_index < 32; byte_index += 8) {
               u32 offsets[256] = {};
               
               for(u32 i = 0; i < count; ++i) {
                    u32 piece = (src[i].key >> byte_index) & 0xFF;
                    ++offsets[piece];
               }
               
               u32 total = 0;
               for(u32 i = 0; i < countof(offsets); ++i) {
                    u32 count = offsets[i];
                    offsets[i] = total;
                    total += count;
               }
               
               for(u32 i = 0; i < count; ++i) {
                    u32 piece = (src[i].key >> byte_index) & 0xFF;
                    dst[offsets[piece]++] = src[i];
               }
               
               swap(dst, src);
          }
     }
}

void seed(rng* rn, u32 seed) {
     rn->seed = seed;
     clear(rn);
}

void clear(rng* rn) {
     rn->state = rn->seed;
}

u32 next_u32(rng* rn) {
     rn->state ^= rn->state << 13;
     rn->state ^= rn->state >> 17;
     rn->state ^= rn->state << 5;
     return rn->state;
}

u64 next_u64(rng* rn) {
     u32 a = next_u32(rn);
     u32 b = next_u32(rn);
     return pack_u64_x2(a, b);
}

s32 next_s32(rng* rn) {
     return (s32)next_u32(rn);
}

s64 next_s64(rng* rn) {
     return (s64)next_u64(rn);
}

f32 next_f32(rng* rn) {
     return bilateral_f32(rn) * F32_MAX;
}

f64 next_f64(rng* rn) {
     return bilateral_f64(rn) * F64_MAX;
}

u32 range_u32(rng* rn, u32 minimum, u32 maximum) {
     return (next_u32(rn) % ((maximum + 1) - minimum)) + minimum;
}

u64 range_u64(rng* rn, u64 minimum, u64 maximum) {
     return (next_u64(rn) % ((maximum + 1) - minimum)) + minimum;
}

s32 range_s32(rng* rn, s32 minimum, s32 maximum) {
     return s32(next_u32(rn) % ((maximum + 1) - minimum)) + minimum;
}

s64 range_s64(rng* rn, s64 minimum, s64 maximum) {
     return s64(next_u32(rn) % ((maximum + 1) - minimum)) + minimum;
}

f32 range_f32(rng* rn, f32 minimum, f32 maximum) {
     return minimum + (unilateral_f32(rn) * (maximum - minimum));
}

f64 range_f64(rng* rn, f64 minimum, f64 maximum) {
     return minimum + (unilateral_f64(rn) * (maximum - minimum));
}

b8x chance(rng* rn, u32 chance) {
     return (next_u32(rn) % chance) == 0;
}

f32 unilateral_f32(rng* rn) {
     return next_u32(rn) / (f32)U32_MAX;
}

f32 bilateral_f32(rng* rn) {
     return unilateral_f32(rn) * 2.0f - 1.0f;
}

f64 unilateral_f64(rng* rn) {
     return next_u64(rn) / (f64)U64_MAX;
}

f64 bilateral_f64(rng* rn) {
     return unilateral_f64(rn) * 2.0f - 1.0f;
}