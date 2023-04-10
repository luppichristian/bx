
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

#if COMPILER == MSVC
#include <intrin.h>
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

u8 safe_u16_to_u8(u16 x) {
     assert(x <= U8_MAX);
     return (u8)x;
}

u16 safe_u32_to_u16(u32 x) {
     assert(x <= U16_MAX);
     return (u16)x;
}

u32 safe_u64_to_u64(u64 x) {
     assert(x <= U32_MAX);
     return (u32)x;
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

u32 rotate_left(u32 x, u32 shift) {
#if COMPILER == MSVC
     return _rotl(x, shift);
#endif
}

u32 rotate_right(u32 x, u32 shift) {
#if COMPILER == MSVC
     return _rotr(x, shift);
#endif
}

u64 rotate_left(u64 x, u64 shift) {
#if COMPILER == MSVC
     return _rotl64(x, shift);
#endif
}

u64 rotate_right(u64 x, u64 shift) {
#if COMPILER == MSVC
     return _rotr64(x, shift);
#endif
}

s16 eswap(s16 x) {
#if COMPILER == MSVC
     return (s16)eswap((u16)x);
#endif
}

u16 eswap(u16 x) {
#if COMPILER == MSVC
     return _byteswap_ushort(x);
#endif
}

s32 eswap(s32 x) {
#if COMPILER == MSVC
     return (s32)eswap((u32)x);
#endif
}

u32 eswap(u32 x) {
#if COMPILER == MSVC
     return _byteswap_ulong(x);
#endif
}

s64 eswap(s64 x) {
#if COMPILER == MSVC
     return (s64)eswap((u64)x);
#endif
}

u64 eswap(u64 x) {
#if COMPILER == MSVC
     return _byteswap_uint64(x);
#endif
}

void eswap(s16* x) {
     *x = eswap(*x);
}

void eswap(u16* x) {
     *x = eswap(*x);
}

void eswap(s32* x) {
     *x = eswap(*x);
}

void eswap(u32* x) {
     *x = eswap(*x);
}

void eswap(s64* x) {
     *x = eswap(*x);
}

void eswap(u64* x) {
     *x = eswap(*x);
}

s16 int_increment(volatile s16* x) {
#if COMPILER == MSVC
     return _InterlockedIncrement16((volatile short*)x) - 1;
#endif
}

u16 int_increment(volatile u16* x) {
#if COMPILER == MSVC
     return (u16)_InterlockedIncrement16((volatile short*)x) - 1;
#endif
}

s32 int_increment(volatile s32* x) {
#if COMPILER == MSVC
     return _InterlockedIncrement((volatile long*)x) - 1;
#endif
}

u32 int_increment(volatile u32* x) {
#if COMPILER == MSVC
     return (u32)_InterlockedIncrement((volatile long*)x) - 1;
#endif
}

s64 int_increment(volatile s64* x) {
#if COMPILER == MSVC
     return _InterlockedIncrement64((volatile __int64*)x) - 1;
#endif
}

u64 int_increment(volatile u64* x) {
#if COMPILER == MSVC
     return (u64)_InterlockedIncrement64((volatile __int64*)x) - 1;
#endif
}

s16 int_decrement(volatile s16* x) {
#if COMPILER == MSVC
     return _InterlockedDecrement16((volatile short*)x) + 1;
#endif
}

u16 int_decrement(volatile u16* x) {
#if COMPILER == MSVC
     return (u16)_InterlockedDecrement16((volatile short*)x) + 1;
#endif
}

s32 int_decrement(volatile s32* x) {
#if COMPILER == MSVC
     return _InterlockedDecrement((volatile long*)x) + 1;
#endif
}

u32 int_decrement(volatile u32* x) {
#if COMPILER == MSVC
     return (u32)_InterlockedDecrement((volatile long*)x) + 1;
#endif
}

s64 int_decrement(volatile s64* x) {
#if COMPILER == MSVC
     return _InterlockedDecrement64((volatile __int64*)x) + 1;
#endif
}

u64 int_decrement(volatile u64* x) {
#if COMPILER == MSVC
     return (u64)_InterlockedDecrement64((volatile __int64*)x) + 1;
#endif
}

s16 int_compare_exchange(volatile s16* x, s16 compare_to, s16 exchange_value) {
#if COMPILER == MSVC
     return _InterlockedCompareExchange16((volatile short*)x, exchange_value, compare_to);
#endif
}

u16 int_compare_exchange(volatile u16* x, u16 compare_to, u16 exchange_value) {
#if COMPILER == MSVC
     return (u16)_InterlockedCompareExchange16((volatile short*)x, exchange_value, compare_to);
#endif
}

s32 int_compare_exchange(volatile s32* x, s32 compare_to, s32 exchange_value) {
#if COMPILER == MSVC
     return _InterlockedCompareExchange((volatile long*)x, exchange_value, compare_to);
#endif
}

u32 int_compare_exchange(volatile u32* x, u32 compare_to, u32 exchange_value) {
#if COMPILER == MSVC
     return (u32)_InterlockedCompareExchange((volatile long*)x, exchange_value, compare_to);
#endif
}

s64 int_compare_exchange(volatile s64* x, s64 compare_to, s64 exchange_value) {
#if COMPILER == MSVC
     return _InterlockedCompareExchange64((volatile __int64*)x, exchange_value, compare_to);
#endif
}

u64 int_compare_exchange(volatile u64* x, u64 compare_to, u64 exchange_value) {
#if COMPILER == MSVC
     return (u64)_InterlockedCompareExchange64((volatile __int64*)x, exchange_value, compare_to);
#endif
}

s16 int_exchange(volatile s16* x, s16 exchange_value) {
#if COMPILER == MSVC
     return _InterlockedExchange16((volatile short*)x, exchange_value);
#endif
}

u16 int_exchange(volatile u16* x, u16 exchange_value) {
#if COMPILER == MSVC
     return (u16)_InterlockedExchange16((volatile short*)x, exchange_value);
#endif
}

s32 int_exchange(volatile s32* x, s32 exchange_value) {
#if COMPILER == MSVC
     return _InterlockedExchange((volatile long*)x, exchange_value);
#endif
}

u32 int_exchange(volatile u32* x, u32 exchange_value) {
#if COMPILER == MSVC
     return (u32)_InterlockedExchange((volatile long*)x, exchange_value);
#endif
}

s64 int_exchange(volatile s64* x, s64 exchange_value) {
#if COMPILER == MSVC
     return _InterlockedExchange64((volatile __int64*)x, exchange_value);
#endif
}

u64 int_exchange(volatile u64* x, u64 exchange_value) {
#if COMPILER == MSVC
     return (u64)_InterlockedExchange64((volatile __int64*)x, exchange_value);
#endif
}

u8 least_significant_bit(u32 mask) {
#if COMPILER == MSVC
     u8 index = U8_MAX;
     if(!_BitScanForward((unsigned long*)&index, mask)) {
          index = U8_MAX;
     }
     return index;
#endif
}

u8 most_significant_bit(u32 mask) {
#if COMPILER == MSVC
     u8 index = U8_MAX;
     if(!_BitScanReverse((unsigned long*)&index, mask)) {
          index = U8_MAX;
     }
     return index;
#endif
}

u8 least_significant_bit(u64 mask) {
#if COMPILER == MSVC
     u8 index = U8_MAX;
     if(!_BitScanForward64((unsigned long*)&index, mask)) {
          index = U8_MAX;
     }
     return index;
#endif
}

u8 most_significant_bit(u64 mask) {
#if COMPILER == MSVC
     u8 index = U8_MAX;
     if(!_BitScanReverse64((unsigned long*)&index, mask)) {
          index = U8_MAX;
     }
     return index;
#endif
}

b8x is_unilateral(f32 x) {
     return in_range(x, 0.0f, 1.0f);
}

b8x is_unilateral(f64 x) {
     return in_range(x, 0.0, 1.0);
}

b8x is_bilateral(f32 x) {
     return in_range(x, -1.0f, 1.0f);
}

b8x is_bilateral(f64 x) {
     return in_range(x, -1.0, 1.0);
}

s32 fact(s32 x) {
     return (x <= 0) ? (1) : (x * fact(x - 1));
}

f32 abs(f32 x) {
     u32 val = f32_to_u32(x);
     return f32_from_u32(val & 0x7FFFFFFF);
}

s32 abs(s32 x) {
     return (x + (x >> 31)) ^ (x >> 31);
}

f32 sqrt(f32 _x) {
     f32 x = _x;
     f32 y = 1;
     while(x - y > EPSILON32) {
          x = (x + y) / 2;
          y = _x / x;
     }
     
     return x;
}

f32 nearby(f32 x) {
     x += (x > 0.0f) ? (0.5f): (-0.5f);
     return (f32)((s32)x);
}

f32 round(f32 x) {
     return (f32)((s32)(x+0.5f));
}

f32 rsqrt(f32 x) {
     f32 xhalf = 0.5f * x;
     u32 i = f32_to_u32(x);
     i = 0x5f3759df - (i >> 1);
     x = f32_from_u32(i);
     x = x * (1.5f - xhalf * x * x);
     return x;
}

f32 floor(f32 x) {
     return (f32)((s32)x);
}

f32 ceil(f32 x) {
     s32 i = (s32) x;
     f32 r = (f32) i;
     r += (f32) (x > r);
     return r;
}

f32 sin(f32 x) {
     f32 res = 0.0f;
     for(u32 i = 0; i < 8; i++)  { 
          res += pow(-1, i) * pow(x, 2 * i + 1) / fact(2 * i + 1);
     }
     
     return res;
}

f32 cos(f32 x) {
     f32 res = 0.0f;
     for(u32 i = 0; i < 8; i++)  { 
          res += pow(-1, i) * pow(x, 2 * i) / fact(2 * i);
     }
     
     return res;
}

f32 pow(f32 base, s32 exp) {
     if(exp < 0) {
          assert(base);
          return 1 / (base * pow(base, (-exp) - 1));
     } else if(exp == 0) {
          return 1;
     } else if(exp == 1) {
          return base;
     }
     
     return base * pow(base, exp - 1);
}

f32 sign(f32 x) {
     return x > 0.0f ? 1.0f : -1.0f;
}

f32 io0(f32 x) {
     return (x != 0.0f) ? 1.0f / x : 0.0f;
}

f32 so0(f32 x) {
     return bias_compare(x, 0.0f) ? 0.0f : sign(x);
}

f32 distance(f32 x, f32 y) {
     return abs(y - x);
}

f32 square(f32 x) {
     return x*x;
}

f32 cube(f32 x) {
     return x*x*x;
}

b8x bias_compare(f32 x, f32 y, f32 bias) {
     return abs(x - y) <= bias;
}

b8x bias_in_range(f32 x, f32 minimum, f32 maximum, f32 bias) {
     return (x >= (minimum - bias)) && (x <= (maximum + bias));
}

f32 bilateral_to_unilateral(f32 bilateral) {
     return bilateral * 0.5f + 0.5f;
}

f32 unilateral_to_bilateral(f32 unilateral) {
     return unilateral * 2.0f - 1.0f;
}

f32 lerp(f32 a, f32 b, f32 t) {
     return (1.0f - t)*a + t*b;
}

f32 deg_to_rad(f32 deg) {
     return deg * (PI32 / 180.0f);
}

f32 rad_to_deg(f32 rad) {
     return rad * (180.0f / PI32);
}

f32 round_to_multiple(f32 x, f32 multiple) {
     return round(x / multiple) * multiple;
}

v2 V2(void) {
     return {};
}

v2 V2(f32 a) {
     return {a, a};
}

v2 V2(f32 a, f32 b) {
     return {a,b};
}

v2 operator+(v2 a, v2 b) {
     return {a.x+b.x, a.y+b.y};
}

v2 operator-(v2 a, v2 b) {
     return {a.x-b.x, a.y-b.y};
}

v2 operator*(v2 a, v2 b) {
     return {a.x*b.x, a.y*b.y};
}

v2 operator/(v2 a, v2 b) {
     return {a.x/b.x, a.y/b.y};
}

v2 operator+(v2 a, f32 b) {
     return a + V2(b);
}

v2 operator-(v2 a, f32 b) {
     return a - V2(b);
}

v2 operator*(v2 a, f32 b) {
     return a * V2(b);
}

v2 operator/(v2 a, f32 b) {
     return a / V2(b);
}

v2 operator+(f32 a, v2 b) {
     return V2(a) + b;
}

v2 operator-(f32 a, v2 b) {
     return V2(a) - b;
}

v2 operator*(f32 a, v2 b) {
     return V2(a) * b;
}

v2 operator/(f32 a, v2 b) {
     return V2(a) / b;
}

v2 operator+=(v2& a, v2 b) {
     return a = a + b;
}

v2 operator-=(v2& a, v2 b) {
     return a = a - b;
}

v2 operator*=(v2& a, v2 b) {
     return a = a * b;
}

v2 operator/=(v2& a, v2 b) {
     return a = a / b;
}

v2 operator+=(v2& a, f32 b) {
     return a = a + b;
}

v2 operator-=(v2& a, f32 b) {
     return a = a - b;
}

v2 operator*=(v2& a, f32 b) {
     return a = a * b;
}

v2 operator/=(v2& a, f32 b) {
     return a = a / b;
}

b8x operator>(v2 a, v2 b) {
     return (a.x>b.x)&& (a.y>b.y);
}

b8x operator<(v2 a, v2 b) {
     return (a.x<b.x)&& (a.y<b.y);
}

b8x operator>=(v2 a, v2 b) {
     return (a.x>=b.x)&& (a.y>=b.y);
}

b8x operator<=(v2 a, v2 b) {
     return (a.x<=b.x)&& (a.y<=b.y);
}

v2 operator+(v2 a) {
     return {+a.x, +a.y};
}

v2 operator-(v2 a) {
     return {-a.x, -a.y};
}

f32 inner(v2 a, v2 b) {
     return a.x*b.x + a.y*b.y;
}

f32 lensq(v2 x) {
     return inner(x, x);
}

f32 len(v2 x) {
     return sqrt(lensq(x));
}

f32 distancesq(v2 a, v2 b) {
     return square(a.x-b.x) + square(a.y-b.y);
}

f32 distance(v2 a, v2 b) {
     return sqrt(distancesq(a, b));
}

v2 round_to_multiple(v2 x, f32 multiple) {
     return {round_to_multiple(x.x, multiple), round_to_multiple(x.y, multiple)};
}

v2 normalize(v2 x) {
     f32 l = len(x);
     return x * io0(l);
}

v2 abs(v2 x) {
     return {abs(x.x), abs(x.y)};
}

v2 perp(v2 x) {
     return {-x.y, x.x};
}

v2 sqrt(v2 x) {
     return {sqrt(x.x), sqrt(x.y)};
}

v2 lerp(v2 a, v2 b, v2 t) {
     return {lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y)};
}

v2 lerp(v2 a, v2 b, f32 t) {
     return lerp(a, b, V2(t));
}

v2 unilateral_to_bilateral(v2 x) {
     return {unilateral_to_bilateral(x.x), unilateral_to_bilateral(x.y)};
}

v2 bilateral_to_unilateral(v2 x) {
     return {bilateral_to_unilateral(x.x), bilateral_to_unilateral(x.y)};
}

v2 min2(v2 a, v2 b) {
     return{min(a.x, b.x), min(a.y, b.y)};
}

v2 max2(v2 a, v2 b) {
     return{max(a.x, b.x), max(a.y, b.y)};
}

v2 clamp2(v2 x, v2 minimum, v2 maximum) {
     return{clamp(x.x, minimum.x, maximum.x), clamp(x.y, minimum.y, maximum.y)};
}

v2 rotate(v2 v, f32 angle) {
     f32 s = sin(angle);
     f32 c = cos(angle);
     return V2(v.x * c - v.y * s, v.x * s + v.y * c);
}

v2 nearby(v2 x) {
     return {nearby(x.x), nearby(x.y)};
}

v2 round(v2 x) {
     return {round(x.x), round(x.y)};
}

v3 V3(void) {
     return {};
}

v3 V3(f32 x) {
     return {x,x,x};
}

v3 V3(f32 x, f32 y, f32 z) {
     return {x,y,z};
}

v3 V3(v2 xy, f32 z) {
     return {xy.x, xy.y, z};
}

v3 V3(f32 x, v2 yz) {
     return {x, yz.x, yz.y};
}

v3 operator+(v3 a, v3 b) {
     return {a.x+b.x, a.y+b.y, a.z+b.z};
}

v3 operator-(v3 a, v3 b) {
     return {a.x-b.x, a.y-b.y, a.z-b.z};
}

v3 operator*(v3 a, v3 b) {
     return {a.x*b.x, a.y*b.y, a.z*b.z};
}

v3 operator/(v3 a, v3 b) {
     return {a.x/b.x, a.y/b.y, a.z/b.z};
}

v3 operator+(v3 a, f32 b) {
     return a + V3(b);
}

v3 operator-(v3 a, f32 b) {
     return a - V3(b);
}

v3 operator*(v3 a, f32 b) {
     return a * V3(b);
}

v3 operator/(v3 a, f32 b) {
     return a / V3(b);
}

v3 operator+(f32 a, v3 b) {
     return V3(a) + b;
}

v3 operator-(f32 a, v3 b) {
     return V3(a) - b;
}

v3 operator*(f32 a, v3 b) {
     return V3(a) * b;
}

v3 operator/(f32 a, v3 b) {
     return V3(a) / b;
}

v3 operator+=(v3& a, v3 b) {
     return a = a + b;
}

v3 operator-=(v3& a, v3 b) {
     return a = a - b;
}

v3 operator*=(v3& a, v3 b) {
     return a = a * b;
}

v3 operator/=(v3& a, v3 b) {
     return a = a / b;
}

v3 operator+=(v3& a, f32 b) {
     return a = a + b;
}

v3 operator-=(v3& a, f32 b) {
     return a = a - b;
}

v3 operator*=(v3& a, f32 b) {
     return a = a * b;
}

v3 operator/=(v3& a, f32 b) {
     return a = a / b;
}

b8x operator>(v3 a, v3 b) {
     return (a.x>b.x)&& (a.y>b.y) && (a.z>b.z);
}

b8x operator<(v3 a, v3 b) {
     return (a.x<b.x)&& (a.y<b.y) && (a.z<b.z);
}

b8x operator>=(v3 a, v3 b) {
     return (a.x>=b.x)&& (a.y>=b.y) && (a.z>=b.z);
}

b8x operator<=(v3 a, v3 b) {
     return (a.x<=b.x)&& (a.y<=b.y) && (a.z<=b.z);
}

v3 operator+(v3 a) {
     return {+a.x, +a.y, +a.z};
}

v3 operator-(v3 a) {
     return {-a.x, -a.y, -a.z};
}

v3 cross(v3 a, v3 b) {
     return {
          a.y * b.z - a.z * b.y,
          a.z * b.x - a.x * b.z,
          a.x * b.y - a.y * b.x,
     };
}

f32 inner(v3 a, v3 b) {
     return a.x*b.x + a.y*b.y + a.z*b.z;
}

f32 lensq(v3 x) {
     return inner(x, x);
}

f32 len(v3 x) {
     return sqrt(lensq(x));
}

f32 distancesq(v3 a, v3 b) {
     return square(a.x-b.x) + square(a.y-b.y) + square(a.z-b.z);
}

f32 distance(v3 a, v3 b) {
     return sqrt(distancesq(a, b));
}

v3 round_to_multiple(v3 x, f32 multiple) {
     return {round_to_multiple(x.x, multiple), round_to_multiple(x.y, multiple), round_to_multiple(x.z, multiple)};
}

v3 normalize(v3 x) {
     f32 l = len(x);
     return x * io0(l);
}

v3 abs(v3 x) {
     return {abs(x.x), abs(x.y), abs(x.z)};
}

v3 sqrt(v3 x) {
     return {sqrt(x.x), sqrt(x.y)};
}

v3 lerp(v3 a, v3 b, v3 t) {
     return {lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y), lerp(a.z, b.z, t.z)};
}

v3 lerp(v3 a, v3 b, f32 t) {
     return lerp(a, b, V3(t));
}

v3 unilateral_to_bilateral(v3 x) {
     return {unilateral_to_bilateral(x.x), unilateral_to_bilateral(x.y), unilateral_to_bilateral(x.z)};
}

v3 bilateral_to_unilateral(v3 x) {
     return {bilateral_to_unilateral(x.x), bilateral_to_unilateral(x.y), bilateral_to_unilateral(x.z)};
}

v3 min2(v3 a, v3 b) {
     return{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)};
}

v3 max2(v3 a, v3 b) {
     return{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
}

v3 clamp2(v3 x, v3 minimum, v3 maximum) {
     return{clamp(x.x, minimum.x, maximum.x), clamp(x.y, minimum.y, maximum.y), clamp(x.z, minimum.z, maximum.z)};
}

v3 nearby(v3 x) {
     return {nearby(x.x), nearby(x.y), nearby(x.z)};
}

v3 round(v3 x) {
     return {round(x.x), round(x.y), round(x.z)};
}

v4 V4(void) {
     return {};
}

v4 V4(f32 x) {
     return {x,x,x,x};
}

v4 V4(f32 x, f32 y, f32 z, f32 w) {
     return {x,y,z,w};
}

v4 V4(v2 xy, v2 zw) {
     return {xy.x,xy.y,zw.x,zw.y};
}

v4 V4(f32 x, v2 yz, f32 w) {
     return {x,yz.x,yz.y,w};
}

v4 V4(v3 xyz, f32 w) {
     return {xyz.x, xyz.y, xyz.z, w};
}

v4 V4(f32 x, v3 yzw) {
     return {x, yzw.x, yzw.y, yzw.z};
}

v4 operator+(v4 a, v4 b) {
     return {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
}

v4 operator-(v4 a, v4 b) {
     return {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};
}

v4 operator*(v4 a, v4 b) {
     return {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
}

v4 operator/(v4 a, v4 b) {
     return {a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};
}

v4 operator+(v4 a, f32 b) {
     return a + V4(b);
}

v4 operator-(v4 a, f32 b) {
     return a - V4(b);
}

v4 operator*(v4 a, f32 b) {
     return a * V4(b);
}

v4 operator/(v4 a, f32 b) {
     return a / V4(b);
}

v4 operator+(f32 a, v4 b) {
     return V4(a) + b;
}

v4 operator-(f32 a, v4 b) {
     return V4(a) - b;
}

v4 operator*(f32 a, v4 b) {
     return V4(a) * b;
}

v4 operator/(f32 a, v4 b) {
     return V4(a) / b;
}

v4 operator+=(v4& a, v4 b) {
     return a = a + b;
}

v4 operator-=(v4& a, v4 b) {
     return a = a - b;
}

v4 operator*=(v4& a, v4 b) {
     return a = a * b;
}

v4 operator/=(v4& a, v4 b) {
     return a = a / b;
}

v4 operator+=(v4& a, f32 b) {
     return a = a + b;
}

v4 operator-=(v4& a, f32 b) {
     return a = a - b;
}

v4 operator*=(v4& a, f32 b) {
     return a = a * b;
}

v4 operator/=(v4& a, f32 b) {
     return a = a / b;
}

b8x operator>(v4 a, v4 b) {
     return (a.x>b.x)&& (a.y>b.y) && (a.z>b.z) && (a.w>b.w);
}

b8x operator<(v4 a, v4 b) {
     return (a.x<b.x)&& (a.y<b.y) && (a.z<b.z) && (a.w<b.w);
}

b8x operator>=(v4 a, v4 b) {
     return (a.x>=b.x)&& (a.y>=b.y) && (a.z>=b.z) && (a.w>=b.w);
}

b8x operator<=(v4 a, v4 b) {
     return (a.x<=b.x)&& (a.y<=b.y) && (a.z<=b.z) && (a.w<=b.w);
}

v4 operator+(v4 a) {
     return {+a.x, +a.y, +a.z, +a.w};
}

v4 operator-(v4 a) {
     return {-a.x, -a.y, -a.z, -a.w};
}


f32 inner(v4 a, v4 b) {
     return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

f32 lensq(v4 x) {
     return inner(x, x);
}

f32 len(v4 x) {
     return sqrt(lensq(x));
}

f32 distancesq(v4 a, v4 b) {
     return square(a.x-b.x) + square(a.y-b.y) + square(a.z-b.z) + square(a.w-b.w);
}

f32 distance(v4 a, v4 b) {
     return sqrt(distancesq(a, b));
}

v4 round_to_multiple(v4 x, f32 multiple) {
     return {round_to_multiple(x.x, multiple), round_to_multiple(x.y, multiple), round_to_multiple(x.z, multiple), round_to_multiple(x.w, multiple)};
}

v4 normalize(v4 x) {
     f32 l = len(x);
     return x * io0(l);
}

v4 abs(v4 x) {
     return {abs(x.x), abs(x.y), abs(x.z), abs(x.w)};
}

v4 sqrt(v4 x) {
     return {sqrt(x.x), sqrt(x.y)};
}

v4 lerp(v4 a, v4 b, v4 t) {
     return {lerp(a.x, b.x, t.x), lerp(a.y, b.y, t.y), lerp(a.z, b.z, t.z), lerp(a.w, b.w, t.w)};
}

v4 lerp(v4 a, v4 b, f32 t) {
     return lerp(a, b, V4(t));
}

v4 unilateral_to_bilateral(v4 x) {
     return {unilateral_to_bilateral(x.x), unilateral_to_bilateral(x.y), unilateral_to_bilateral(x.z), unilateral_to_bilateral(x.w)};
}

v4 bilateral_to_unilateral(v4 x) {
     return {bilateral_to_unilateral(x.x), bilateral_to_unilateral(x.y), bilateral_to_unilateral(x.z), bilateral_to_unilateral(x.w)};
}

v4 min2(v4 a, v4 b) {
     return{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)};
}

v4 max2(v4 a, v4 b) {
     return{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)};
}

v4 clamp2(v4 x, v4 minimum, v4 maximum) {
     return{clamp(x.x, minimum.x, maximum.x), clamp(x.y, minimum.y, maximum.y), clamp(x.z, minimum.z, maximum.z), clamp(x.w, minimum.w, maximum.w)};
}

v4 nearby(v4 x) {
     return {nearby(x.x), nearby(x.y), nearby(x.z), nearby(x.w)};
}

v4 round(v4 x) {
     return {round(x.x), round(x.y), round(x.z), round(x.w)};
}
