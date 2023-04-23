#ifndef PLATFORMDEFINE_HPP
#define PLATFORMDEFINE_HPP

#define PLATFORM_UNKNOWN 0
#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC     2
#define PLATFORM_LINUX   3

#define TARGET_PLATFORM       PLATFORM_UNKNOWN

#if defined(_WIN32)
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM   PLATFORM_WINDOWS
#endif

#if defined(__APPLE__)
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM   PLATFORM_MAC
#endif

#if defined(linux) && !defined(__APPLE__)
    #undef TARGET_PLATFORM
    #define TARGET_PLATFORM   PLATFORM_MAC
#endif

#endif /* PLATFORMDEFINE_HPP */