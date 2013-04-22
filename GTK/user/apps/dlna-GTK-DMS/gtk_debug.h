#ifndef _GTK_DEBUG_H_
#define _GTK_DEBUG_H_

#define GTK_DEBUG

#ifdef GTK_DEBUG
        #define GTKDBG(fmt,args...) \
        do { \
           FILE *fp = fopen("/dev/console", "w"); \
           if (fp) { \
              fprintf(fp,"%s(%d) :: \n",__FUNCTION__, __LINE__); \
              fprintf(fp, fmt, ## args); \
              fclose(fp); \
           }\
        } while (0)

	#define log_verbose  GTKDBG
        #define log_error    GTKDBG
        #define log_info     GTKDBG

#else
        #define GTKDBG(fmt,args...)
#endif

#endif


