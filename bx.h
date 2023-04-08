
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

#pragma once

// ***

// The first section is dedicated to platform detection, architecture detection and compiler detection.
// Everything is detected with pre-defined macros.

// *********

// List of the supported compilers.
#define CLANG 0x01
#define MSVC  0x02
#define GCC   0x03

// Detect the current compiler.
#if defined(__clang__)
#define COMPILER CLANG
#elif defined(_MSC_VER)
#define COMPILER MSVC
#elif defined(__GNUC__)
#define COMPILER GCC
#endif

#if !defined(COMPILER)
#error Unknown compiler!
#endif

// *********

// List of the supported platforms.
#define WIN32 0x01
#define LINUX 0x02
#define MACOS 0x03

// Detect the current platform.
#if defined(_WIN32)
#define PLATFORM WIN32
#elif defined(__APPLE__) && defined(__MACH__)
#define PLATFORM MACOS
#elif defined(__gnu_linux__) || defined(__linux__)
#define PLATFORM LINUX
#endif

#if !defined(PLATFORM)
#error Unknown platform!
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
#elif defined(_M_I86)
#define ARCHITECTURE X86
#elif defined(_M_ARM)
#define ARCHITECTURE ARM
#elif defined(_M_ARM64)
#define ARCHITECTURE ARM64
#endif

#elif COMPILER == CLANG
#if defined(__amd64__)
#define ARCHITECTURE X64
#elif defined(__i386__)
#define ARCHITECTURE X86
#elif defined(__arm__)
#define ARCHITECTURE ARM
#elif defined(__aarch64__)
#define ARCHITECTURE ARM64
#endif

#elif COMPILER == GCC
#if defined(__amd64__)
#define ARCHITECTURE X64
#elif defined(__i386__)
#define ARCHITECTURE X86
#elif defined(__arm__)
#define ARCHITECTURE ARM
#elif defined(__aarch64__)
#define ARCHITECTURE ARM64
#endif

#endif

#if !defined(ARCHITECTURE)
#error Unknown architecture!
#endif