#ifndef PESTACLE_CMDLINE_H
#define PESTACLE_CMDLINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


typedef struct {
	bool dry_run;
	bool profile_mode;
	int frames_per_second;
	char* input_path;
} CmdParameters;


extern void
CmdParameters_init(
	CmdParameters* self
);


extern void
CmdParameters_destroy(
	CmdParameters* self
);


extern bool
CmdParameters_parse(
	CmdParameters* self,
	int argc,
	char* argv[]
);


#ifdef __cplusplus
}
#endif

#endif /* PESTACLE_CMDLINE_H */
