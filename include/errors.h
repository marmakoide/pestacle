#ifndef PESTACLE_ERRORS_H
#define PESTACLE_ERRORS_H

#include <stdnoreturn.h>
#include "file_location.h"


extern noreturn void
handle_out_of_memory_error();


extern noreturn void
handle_input_read_error();


extern noreturn void 
handle_processing_error(
	const FileLocation* location,
	const char* format,
	...
);


#endif // PESTACLE_ERRORS_H 