#pragma once
/* counter.h --- public interface to counter module
 * 
 * 
 * Author: David W. Domonoske
 * Created: Wed Nov 25 18:38:13 2020 (-0500)
 * Version: 1.0
 * 
 * Description: keep track of a count 
 *  - every function, except open and close, return the newest count
 */

#include <stdint.h>
//#include <stdbool.h>


/* the counter representation is hidden from users of the module */
typedef void counter_t;

/* create a new counter with count=0
 * return pointer to new counter
 */
counter_t* openCount(void);

/* deallocate a counter */
void closeCount(counter_t *cp);

/* get current count
 */
int32_t getCount(counter_t *cp);

/* increase count by 1
 * return new count
 */
int32_t incCount(counter_t *cp);

/* decrease count by 1
 * return new count
 */
int32_t decCount(counter_t *cp);

/* set count to x
 * return new count (will always be x, but why not)
 */
int32_t setCount(counter_t *cp, int32_t x);

/* reset count to 0
 * return new count (will always be 0, but why not)
 */
int32_t resetCount(counter_t *cp);

/* add n to current count
 */
int32_t addCount(counter_t *cp, int32_t x);

