#ifndef PESTACLE_MACROS_H
#define PESTACLE_MACROS_H


/*
 * Macro to tag functions parameters as unused without triggering a warning
 */

#ifdef __GNUC__
#define ATTRIBUTE_UNUSED __attribute__((unused))
#else
#define ATTRIBUTE_UNUSED
#endif


#endif /* PESTACLE_MACROS_H */
