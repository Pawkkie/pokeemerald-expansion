#ifndef METAPROGRAM_H
#define METAPROGRAM_H/* Picks the xth VA_ARG if it exists, otherwise returns a default value */

/* Picks the xth VA_ARG if it exists, otherwise returns a default value */
#define DEFAULT(_default, ...) FIRST(__VA_OPT__(__VA_ARGS__, ) _default)
#define DEFAULT_2(_default, ...) DEFAULT(_default __VA_OPT__(, SECOND(__VA_ARGS__)))
#define DEFAULT_3(_default, ...) DEFAULT(_default __VA_OPT__(, THIRD(__VA_ARGS__)))
#define DEFAULT_4(_default, ...) DEFAULT(_default __VA_OPT__(, FOURTH(__VA_ARGS__)))

/* Expands to the first/second/third/fourth argument. */
#define FIRST(a, ...) a
#define SECOND(a, ...) __VA_OPT__(FIRST(__VA_ARGS__))
#define THIRD(a, ...) __VA_OPT__(SECOND(__VA_ARGS__))
#define FOURTH(a, ...) __VA_OPT__(THIRD(__VA_ARGS__))

#endif