#ifndef _DEBUG_H_
#define _DEBUG_H_

/*
 * It's necessary to define NDEBUG before including assert.h.
 */
#ifndef DEBUG
#define NDEBUG
#endif /* DEBUG */
#include <assert.h>

/*
 * Print when the DEBUG macro is defined.
 */
#ifdef DEBUG
#include <stdio.h>
#define __DEBUG_MSG(tag, ...) \
{ \
	printf(tag ": %20s:%5d: ", __FILE__, __LINE__); \
	printf(__VA_ARGS__); \
	printf("\n"); \
}
#define DEBUG_ERROR(...) __DEBUG_MSG("E", __VA_ARGS__)
#define DEBUG_WARN(...)  __DEBUG_MSG("W", __VA_ARGS__)
#define DEBUG_TRACE(...) __DEBUG_MSG("T", __VA_ARGS__)
#define DEBUG_INFO(...)  __DEBUG_MSG("I", __VA_ARGS__)
#define DEBUG_ONCE(...)  {static int __oncevar=1; if (__oncevar) {__DEBUG_MSG("O", __VA_ARGS__); __oncevar=0;}}
#define BARF() \
{ \
	printf("%s:%d\n", __FILE__, __LINE__); \
}
#else /* DEBUG */
#define DEBUG_ERROR(...){}
#define DEBUG_WARN(...) {}
#define DEBUG_TRACE(...){}
#define DEBUG_INFO(...) {}
#define DEBUG_ONCE(...) {}
#endif /* DEBUG */

#endif /* _DEBUG_H_ */
