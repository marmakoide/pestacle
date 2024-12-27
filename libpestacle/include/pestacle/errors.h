#ifndef PESTACLE_ERRORS_H
#define PESTACLE_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdnoreturn.h>
#include <pestacle/file_location.h>


extern noreturn void
handle_out_of_memory_error();


extern noreturn void
handle_input_read_error();


extern void
log_error(
	const char* format,
	...
);


#define handle_parsing_error(location, format, ...) \
	log_error("line %d : " format, (location)->line + 1, __VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_ERRORS_H */
