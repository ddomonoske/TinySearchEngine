#pragma once
/* lcounter.h --- public interface to locked counter module
 * 
 * 
 * Author: David W. Domonoske
 * Created: Wed Nov 25 18:38:13 2020 (-0500)
 * Version: 1.0
 * 
 * Description: keep track of count
 *  - exact copy of counter module, but with locks
 *  - lock counter anytime the count is being changed 
 *  - every function, except open and close, returns the newest count
 */

#include "counter.h"


/* the locked counter representation is hidden from users of the module */
typedef void lcounter_t;

/* create a new counter with count=0
 * return pointer to new counter
 */
lcounter_t* openLCount(void);

/* deallocate a counter */
void closeLCount(lcounter_t *cp);

/* get current count
 */
int32_t getLCount(lcounter_t *cp);

/* increase count by 1
 * return new count
 */
int32_t incLCount(lcounter_t *cp);

/* decrease count by 1
 * return new count
 */
int32_t decLCount(lcounter_t *cp);

/* set count to x
 * return new count (will always be x, but why not)
 */
int32_t setLCount(lcounter_t *cp, int32_t x);

/* reset count to 0
 * return new count (will always be 0, but why not)
 */
int32_t resetLCount(lcounter_t *cp);

/* add n to current count
 * return new count
 */
int32_t addLCount(lcounter_t *cp, int32_t x);

