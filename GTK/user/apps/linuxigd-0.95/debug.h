
#ifdef GMTK_DEBUG
	#define	GMTK_DBG(fmt, args...) fprintf(stderr,"%s (%d) :" fmt, __FUNCTION__, __LINE__, ##args)
#else
	#define	GMTK_DBG(fmt, args...)
#endif