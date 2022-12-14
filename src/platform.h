#ifndef PLATFORM_H__HEADER_GUARD__
#define PLATFORM_H__HEADER_GUARD__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	define PLATFORM_WINDOWS
#elif defined(__APPLE__)
#	define PLATFORM_APPLE
#elif defined(__linux__) || defined(__gnu_linux__) || defined(linux)
#	define PLATFORM_LINUX
#elif defined(__unix__) || defined(unix)
#	define PLATFORM_UNIX
#else
#	define PLATFORM_UNKNOWN
#endif

#endif
