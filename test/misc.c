#include "minunit.h"

#include <pestacle/macros.h>
#include <pestacle/string_list.h>


// --- StringList testing ----------------------------------------------------

MU_TEST(test_StringList_join) {
	const char* item_a = "tic";
	const char* item_b = "tac";
	const char* item_c = "toc";
	const char* item_d = "";

	// Initialization
	StringList str_list;
	StringList_init(&str_list);

	// Case A => 3 non-null items
	const char* case_a_expected_out = "tic.tac.toc";

	StringList_append(&str_list, item_a);
	StringList_append(&str_list, item_b);
	StringList_append(&str_list, item_c);

	char* case_a_out = StringList_join(&str_list, '.');

	printf("'%s' = > '%s'\n", case_a_expected_out, case_a_out);

	mu_check(strcmp(case_a_out, case_a_expected_out) == 0);
	free(case_a_out);

	// Case B => 1 non-null items
	const char* case_b_expected_out = "tic";

	StringList_clear(&str_list);
	StringList_append(&str_list, item_a);

	char* case_b_out = StringList_join(&str_list, '.');

	printf("'%s' = > '%s'\n", case_b_expected_out, case_b_out);

	mu_check(strcmp(case_b_out, case_b_expected_out) == 0);
	free(case_b_out);

	// Case C => 0 items
	const char* case_c_expected_out = "";

	StringList_clear(&str_list);

	char* case_c_out = StringList_join(&str_list, '.');

	printf("'%s' = > '%s'\n", case_c_expected_out, case_c_out);

	mu_check(strcmp(case_c_out, case_c_expected_out) == 0);
	free(case_c_out);

	// Case D => 2 non-null items, 1 null items
	const char* case_d_expected_out = "tic..toc";

	StringList_clear(&str_list);
	StringList_append(&str_list, item_a);
	StringList_append(&str_list, item_d);
	StringList_append(&str_list, item_c);

	char* case_d_out = StringList_join(&str_list, '.');

	printf("'%s' = > '%s'\n", case_d_expected_out, case_d_out);

	mu_check(strcmp(case_d_out, case_d_expected_out) == 0);
	free(case_d_out);

	// Job done
	StringList_destroy(&str_list);
}


// --- Main entry point ------------------------------------------------------

MU_TEST_SUITE(test_StringList_suite) {
	MU_RUN_TEST(test_StringList_join);
}


int
main(ATTRIBUTE_UNUSED int argc, ATTRIBUTE_UNUSED char *argv[]) {
	MU_RUN_SUITE(test_StringList_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
