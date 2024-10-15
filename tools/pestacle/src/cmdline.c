#include <stdlib.h>
#include <string.h>
#include <argtable3.h>

#include "cmdline.h"

#include <pestacle/memory.h>


void
CmdParameters_init(
	CmdParameters* self
) {
	self->dry_run = false;
	self->profile_mode = false;
	self->frames_per_second = 60;
	self->timeout = 0;
	self->input_path = 0;
}


void
CmdParameters_destroy(
	CmdParameters* self
) {
	if (self->input_path)
		free(self->input_path);
}


bool
CmdParameters_parse(
	CmdParameters* self,
	int argc,
	char* argv[]
) {
	char prog_name[] = "pestacle";

	// Declare the list of arguments
	struct arg_lit*  help;
	struct arg_lit*  dry_run;
	struct arg_lit*  profile_mode;
	struct arg_int*  frames_per_second;
	struct arg_int*  timeout;
	struct arg_file* file;
	struct arg_end*  end;

	void* argtable[] = {
		help              = arg_litn( NULL,       "help",           0, 1, "display this help and exit"),
		dry_run           = arg_litn( NULL,       "dry-run",        0, 1, "load but do not execute the script"),
		profile_mode      = arg_litn( NULL,       "profile",        0, 1, "enable profiling of the executed script"),
		frames_per_second = arg_intn( NULL,       "fps",     "<n>", 0, 1, "frames per seconds"),
		timeout           = arg_intn( NULL,       "timeout", "<n>", 0, 1, "stops after specified number of seconds"),
		file              = arg_filen(NULL, NULL, "<file>",         1, 1, "input script"),
		end               = arg_end(20),
	};

	// Parse the arguments
	int cmd_line_error_count = arg_parse(argc, argv, argtable);

	// Help option handling
	if (help->count > 0) {
		printf("Usage: %s", prog_name);
		arg_print_syntax(stdout, argtable, "\n");

		printf("Runs a pestacle script.\n\n");
		arg_print_glossary(stdout, argtable, "  %-25s %s\n");

		return false;
	}

	// Errors were triggered
	if (cmd_line_error_count > 0) {
		arg_print_errors(stdout, end, prog_name);
		printf("Try '%s --help' for more information.\n", prog_name);

		return false;
	}

	// Read dry-run mode flag
	if (dry_run->count > 0)
		self->dry_run = true;

	// Read profile mode flag
	if (profile_mode->count > 0)
		self->profile_mode = true;

	// Read frame per seconds
	if (frames_per_second->count > 0)
		self->frames_per_second = frames_per_second->ival[0];

	// Read frame per seconds
	if (timeout->count > 0)
		self->timeout = timeout->ival[0];

	// Read input file path
	size_t input_path_len = strlen(file->filename[0]) + 1;
	self->input_path = (char*)checked_malloc(input_path_len * sizeof(char));
	memcpy(self->input_path, file->filename[0], input_path_len * sizeof(char));

	// Free ressources
	arg_freetable(
		argtable,
		sizeof(argtable) / sizeof(argtable[0])
	);

	// Job done
	return true;
}
