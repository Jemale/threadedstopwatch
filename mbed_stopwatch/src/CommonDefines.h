#ifndef COMMON_DEFINES_H
#define COMMON_DEFINES_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef FALSE
   #define FALSE 0
#endif

#ifndef TRUE
   #define TRUE 1
#endif

#ifndef TEST
   #define STATIC static
   #define MAIN main
   #define FOREVER 1
#else
   #define STATIC
   #define MAIN app_main
   #define FOREVER 0
#endif

#ifdef LINUX
    #ifndef EXIT_FAILURE
	#define EXIT_FAILURE 1
    #endif
    #define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
#endif
#endif /* COMMON_DEFINES_H */
