/*
 *  TRDebug.h
 *  tuxracer
 *
 */

#ifdef TR_DEBUG_MODE
# define TRDebugLog printf
#else
# define TRDebugLog if(0) printf
#endif