
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
#define BX

// *********

// List of the supported compilers.
#define CLANG 0x01
#define MSVC  0x02
#define GCC   0x03

// Detect the current compiler.
#if defined(__clang__)
#define COMPILER CLANG
#define COMPILER_NAME "CLANG"
#elif defined(_MSC_VER)
#define COMPILER MSVC
#define COMPILER_NAME "MSVC"
#elif defined(__GNUC__)
#define COMPILER GCC
#define COMPILER_NAME "GCC"
#endif

#if !defined(COMPILER)
#error Unknown compiler!
#endif
#if !defined(COMPILER_NAME)
#error Unknown compiler name!
#endif

// *********

// List of the supported platforms.
#define WIN32 0x01
#define LINUX 0x02
#define MACOS 0x03

// Detect the current platform.
#if defined(_WIN32)
#define PLATFORM WIN32
#define PLATFORM_NAME "WIN32"
#elif defined(__APPLE__) && defined(__MACH__)
#define PLATFORM MACOS
#define PLATFORM_NAME "MACOS"
#elif defined(__gnu_linux__) || defined(__linux__)
#define PLATFORM LINUX
#define PLATFORM_NAME "LINUX"
#endif

#if !defined(PLATFORM)
#error Unknown platform!
#endif
#if !defined(PLATFORM_NAME)
#error Unknown platform name!
#endif

// *********

// List of the supported architectures.
#define X64   0x01
#define X86   0x02
#define ARM   0x03
#define ARM64 0x04

// Detect the current architecture.
#if COMPILER == MSVC
#if defined(_M_AMD64)
#define ARCHITECTURE X64
#define ARCHITECTURE_NAME "X64"
#elif defined(_M_I86)
#define ARCHITECTURE X86
#define ARCHITECTURE_NAME "X86"
#elif defined(_M_ARM)
#define ARCHITECTURE ARM
#define ARCHITECTURE_NAME "ARM"
#elif defined(_M_ARM64)
#define ARCHITECTURE ARM64
#define ARCHITECTURE_NAME "ARM64"
#endif

#elif COMPILER == CLANG
#if defined(__amd64__)
#define ARCHITECTURE X64
#define ARCHITECTURE_NAME "X64"
#elif defined(__i386__)
#define ARCHITECTURE X86
#define ARCHITECTURE_NAME "X86"
#elif defined(__arm__)
#define ARCHITECTURE ARM
#define ARCHITECTURE_NAME "ARM"
#elif defined(__aarch64__)
#define ARCHITECTURE ARM64
#define ARCHITECTURE_NAME "ARM64"
#endif

#elif COMPILER == GCC
#if defined(__amd64__)
#define ARCHITECTURE X64
#define ARCHITECTURE_NAME "X64"
#elif defined(__i386__)
#define ARCHITECTURE X86
#define ARCHITECTURE_NAME "X86"
#elif defined(__arm__)
#define ARCHITECTURE ARM
#define ARCHITECTURE_NAME "ARM"
#elif defined(__aarch64__)
#define ARCHITECTURE ARM64
#define ARCHITECTURE_NAME "ARM64"
#endif

#endif

#if !defined(ARCHITECTURE)
#error Unknown architecture!
#endif
#if !defined(ARCHITECTURE_NAME)
#error Unknown architecture name!
#endif

// *********

#include <float.h>
#include <stdint.h>

// Numeric types (unsigned fixed length).
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Numeric types (signed fixed length).
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// Numeric types (unsigned fast).
typedef uint_fast8_t  u8x;
typedef uint_fast16_t u16x;
typedef uint_fast32_t u32x;
typedef uint_fast64_t u64x;

// Numeric types (signed fast).
typedef int_fast8_t  s8x;
typedef int_fast16_t s16x;
typedef int_fast32_t s32x;
typedef int_fast64_t s64x;

// Numeric types (memory).
typedef uintptr_t up;
typedef intptr_t  sp;
typedef size_t    sz;

// Numeric types (floating point).
typedef float  f32;
typedef double f64;

// Boolean types (unsigned fixed length).
typedef s8  b8;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

// Boolean types (unsigned fast).
typedef s8x  b8x;
typedef s16x b16x;
typedef s32x b32x;
typedef s64x b64x;

// *********

// Numeric types (unsigned fixed length).
#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX
#define U8_MIN  0
#define U16_MIN 0
#define U32_MIN 0
#define U64_MIN 0

// Numeric types (signed fixed length).
#define S8_MAX  INT8_MAX
#define S16_MAX INT16_MAX
#define S32_MAX INT32_MAX
#define S64_MAX INT64_MAX
#define S8_MIN  INT8_MIN
#define S16_MIN INT16_MIN
#define S32_MIN INT32_MIN
#define S64_MIN INT64_MIN

// Numeric types (unsigned fast).
#define U8X_MAX  UINT_FAST8_MAX
#define U16X_MAX UINT_FAST16_MAX
#define U32X_MAX UINT_FAST32_MAX
#define U64X_MAX UINT_FAST64_MAX
#define U8X_MIN  0
#define U16X_MIN 0
#define U32X_MIN 0
#define U64X_MIN 0

// Numeric types (signed fast).
#define S8X_MAX  INT_FAST8_MAX
#define S16X_MAX INT_FAST16_MAX
#define S32X_MAX INT_FAST32_MAX
#define S64X_MAX INT_FAST64_MAX
#define S8X_MIN  INT_FAST8_MIN
#define S16X_MIN INT_FAST16_MIN
#define S32X_MIN INT_FAST32_MIN
#define S64X_MIN INT_FAST64_MIN

// Numeric types (memory).
#define UP_MAX UINTPTR_MAX
#define SP_MAX INTPTR_MAX
#define SZ_MAX SIZE_MAX
#define UP_MIN 0
#define SP_MIN INTPTR_MIN
#define SZ_MIN 0

// Numeric types (floating point).
#define F32_MAX FLT_MAX
#define F64_MAX DBL_MAX
#define F32_MIN (-F32_MAX)
#define F64_MIN (-F64_MAX)

// *********

// Custom keywords.
#define local_persist static
#define global_variable static
#define internal static
#define entry_point
#define unused
#define c_linkage extern "C"

// Shared library export.
#if COMPILER == MSVC
#define shared_export __declspec(dllexport)
#elif COMPILER == CLANG
#define shared_export __declspec(dllexport)
#elif COMPILER == GCC
#define shared_export __attribute__((visibility("default")))
#endif

// Preprocessor utilities.
#define stringify(x) #x
#define concat(x, y) x##y
#define stringify_exp(x) stringify(x)
#define concat_exp(x,y) concat(x, y)
#define expr(x) do { x; } while(0)
#define countof(x) (sizeof(x)/sizeof((x)[0]))
#define typeof(x) decltype(x)
#define offsetof(str, member) ((&(((str*)(0))->member)))
#define sizeof_each(x) sizeof((x)[0])
#define multiline_literal(...) stringify_exp(__VA_ARGS__)

// Bit utilities.
#define bit(x) (1 << (x))
#define is_bit_set(bits, b) ((bits)&(b))
#define set_bit(bits, b) ((bits)|=(b))
#define unset_bit(bits, b) ((bits)&=~(b))
#define toggle_bit(bits, b) ((bits)^=(b))

// Numeric utilities.
#define kb(b) ((b) * 1024ll)
#define mb(b) (kb(b) * 1024ll)
#define gb(b) (mb(b) * 1024ll)
#define tb(b) (gb(b) * 1024ll)
#define th(x)((x)*1000ll)
#define mil(x) (th(x)*1000ll)
#define bil(x) (mil(x)*1000ll)
#define tril(x) (bil(x)*1000ll)
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define clamp(x, lower, upper) max(lower, min(x, upper))
#define clamp01(x) clamp(x, 0, 1)
#define align_pow2(x, align) (((x) + ((align) - 1)) & ~(((x) - (x)) + (align) - 1))
#define is_pow2(x) (((x) & ((x)-1)) == 0)
#define in_range(x, minimum, maximum) (((x) >= (minimum)) && ((x) <= (maximum)))
#define normalize_bool(x) ((x) > 1 ? 1 : 0)

// Pointers.
#define uint_from_ptr(ptr) (up)((u8*)(ptr) - (u8*)(0))
#define uint_to_ptr(i) (void*)((u8*)0 + (i))
#define int_from_ptr(ptr) (sp)((u8*)(ptr) - (u8*)(0))
#define int_to_ptr(i) (void*)((u8*)0 + (i))

// Variadic arguments.
#include <stdarg.h>
#define var_start(list, fmt) va_start(list, fmt)
#define var_end(list) va_end(list)
#define var_fetch(list, type) va_arg(list, type)

// Debug.
#define debug_break() *(((int*)0)) = 0
#define debug_step() int _______a##__LINE__ = 0

// Assertions.
#define assert(x) expr( if(!(x)) { debug_break(); } )
#define cassert(x) typedef char concat_exp(__FILE__, __LINE__) [(x)?1:-1]
#define assert2(x, eA, eB) expr(if(x) { assert(eA); }; else { assert(eB); };)
#define invalid_path assert(!"INVALID")
#define invalid_default_case default: { invalid_path; } break
#define unimplemented_path assert(!"UNIMPLEMENTED")

// Misc.
#define swap(x, y) expr( auto __z = x; x = y; y = __z; )
#define rswap(x, y) expr( auto __z = *(x); *(x) = *(y); *(y) = __z; )
#define fill(x, val) expr( for(u32x __i = 0; __i < countof(x); ++__i) { x[__i] = (val); }; )
#define circ_increment(val, ival, cap) val = ((val + ival) % cap)
#define circ_decrement(val, dval, cap) (val < dval) ? (cap - dval + val) : (val - dval)
#define assign_nonvolatile(x, val) expr(typeof(x)* temp = (typeof(x)*)&(x); *temp = val;)
#define assign_volatile(x, val) expr(volatile typeof(x)* temp = (volatile typeof(x)*)&(x); *temp = val;)
#define repeat(x) for(u32x __i##__LINE__  = 0; __i##__LINE__ < x; __i##__LINE__++)

// *********

// Singly linked list utils.
#define singly_push_after(first, last, node, previous) if(previous) { if(previous->next) { (node)->next = (previous)->next; (previous)->next = node; } else { singly_push_back(first, last, node); }  } else { singly_push_front(first, last, node); }
#define singly_push_front(first, last, node) (node)->next = first; first = node; if(!last) { last = node; }
#define singly_push_back(first, last, node) node->next = nullptr; if((first) && (last)) { (last)->next = node; last = node; } else { first = last = node; }
#define singly_pop_front(first, last) if(first) { first = (first)->next; if(!first) last = nullptr; }
#define singly_remove(first, last, previous, node) (previous) ? ((previous)->next = (node)->next) : (first = node->next); ((node)->next) ? (0) : (last = previous);

// Doubly linked list utils.
#define doubly_push_after(first, last, node, previous) if(previous) { if(previous->next) { (node)->next = (previous)->next; (previous)->next = node; ((node)->next)->previous = node; } else { doubly_push_back(first, last, node); }  } else { doubly_push_front(first, last, node); } 
#define doubly_push_front(first, last, node) (node)->next = first; if(first) { (first)->previous = node; } (node)->previous = nullptr; first = node; if(!last) { last = node; }
#define doubly_push_back(first, last, node) (last) ? ((last)->next = node) : (first = node); (node)->previous = last; (node)->next = NULL; last = node;
#define doubly_remove(first, last, node) ((node)->previous) ? ((node)->previous->next = (node)->next) : (first = (node)->next); ((node)->next) ? ((node)->next->previous = (node)->previous) : (last = (node)->previous);

// *********

// Integer packing.
#define pack_u64_x2(a, b) ((u64(b) << 32) | u64(a))

// Floating point to integer conversion without casting.
u32 f32_to_u32(f32 x);
u64 f64_to_u64(f64 x);
f32 f32_from_u32(u32 x);
f64 f64_from_u64(u64 x);

// *********

// Basic memory ops.
void* copy(void* dst, void* src, sz size);
void* set8(void* dst, u8 byte, sz count);
void* set16(void* dst, u16 word, sz count); 
void* set32(void* dst, u32 dword, sz count);
void* set64(void* dst, u64 qword, sz count);
void* zero(void* dst, sz size);
void* move(void* dst, void* src, sz size);
void* align(void* ptr, up alignment);
void* align(void* ptr, up padding, up alignment);
bool  compare(void* a, void* b, sz size);

// Macros for basic memory ops.
#define zero_obj(x) (typeof(x))(zero(x, sizeof(*(x)))
#define copy_obj(dst, src) (typeof(dst))(copy(dst, src, sizeof(*(dst))))
#define compare_objs(a, b) compare(a, b, sizeof(*(a)))
#define zero_array(x, count) (typeof(x))(zero(x, sizeof(*(x))  * count)
#define copy_array(dst, src, count) (typeof(dst))(copy(dst, src, sizeof(*(dst))  * count))
#define compare_arrays(a, b, count) compare(a, b, sizeof(*(a)) * count)

// Compression.
sz compress_lz(void* dst, void* src, sz size);
sz compress_rle(void* dst, void* src, sz size);
void decompress_lz(void* dst, void* src, sz size, sz decompressed_size);
void decompress_rle(void* dst, void* src, sz size, sz decompressed_size);

// *********

struct sort_entry {
     u32 key;
     u32 value;
};

// Sort algorithms.
b8x are_sorted(sort_entry* entries, u32 count);
void sort_bubble(sort_entry* entries, u32 count);
void sort_quick(sort_entry* entries, u32 count);
void sort_radix(sort_entry* entries, u32 count);

// *********

struct rng {
     u32 seed;
     u32 state;
};

// Random Number Generation.
void seed(rng* rn, u32 seed);
void clear(rng* rn); // Reset state to seed.

// Absolute values.
u32 next_u32(rng* rn);
u64 next_u64(rng* rn);
s32 next_s32(rng* rn);
s64 next_s64(rng* rn);
f32 next_f32(rng* rn);
f64 next_f64(rng* rn);

// Range values.
u32 range_u32(rng* rn, u32 minimum, u32 maximum); // Min and max are inclusive.
u64 range_u64(rng* rn, u64 minimum, u64 maximum); // Min and max are inclusive.
s32 range_s32(rng* rn, s32 minimum, s32 maximum); // Min and max are inclusive.
s64 range_s64(rng* rn, s64 minimum, s64 maximum); // Min and max are inclusive.
f32 range_f32(rng* rn, f32 minimum, f32 maximum); // Min and max are inclusive.
f64 range_f64(rng* rn, f64 minimum, f64 maximum); // Min and max are inclusive.

// Chance.
b8x chance(rng* rn, u32 chance);

// Floats.
f32 unilateral_f32(rng* rn); // Values between 0 and 1.
f32 bilateral_f32(rng* rn); // Values between -1 and 1.
f64 unilateral_f64(rng* rn); // Values between 0 and 1.
f64 bilateral_f64(rng* rn); // Values between -1 and 1.

#endif