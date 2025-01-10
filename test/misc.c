#include "minunit.h"

#include <stdlib.h>

#include <pestacle/macros.h>
#include <pestacle/memory.h>
#include <pestacle/tree_map.h>
#include <pestacle/string_list.h>


// --- TreeMap testing -------------------------------------------------------

#define TREE_MAP_VALUE_COUNT 1024


MU_TEST(test_TreeMap_insert) {
	TreeMap tree;

	// Initialize data to be stored
	int* values = (int*)checked_malloc(sizeof(int) * TREE_MAP_VALUE_COUNT);
	for(int i = 0; i < TREE_MAP_VALUE_COUNT; ++i)
		values[i] = i;

	// Initialization of the tree map
	TreeMap_init(&tree);

	// Fill the tree map
	for(int i = 0; i < TREE_MAP_VALUE_COUNT; ++i) {
		// Check that all non-inserted elements can not be found
		for(int j = i; j < TREE_MAP_VALUE_COUNT; ++j) {
			TreeMapNode* node = TreeMap_find(&tree, values + i);
			if (node)
				mu_fail("TreeMapNode should not be found");
		}

		// Add the element
		TreeMap_insert(&tree, values + i);

		// Check that all inserted elements can be found
		for(int j = 0; j <= i; ++j) {
			TreeMapNode* node = TreeMap_find(&tree, values + i);
			if (!node)
				mu_fail("TreeMapNode should be found");

			mu_check(node->key == values + i);
		}
	}


	// Release ressources
	TreeMap_destroy(&tree);
	free(values);
}


MU_TEST(test_TreeMap_erase) {
	TreeMap tree;

	// Initialize data to be stored
	int* values = (int*)checked_malloc(sizeof(int) * TREE_MAP_VALUE_COUNT);
	for(int i = 0; i < TREE_MAP_VALUE_COUNT; ++i)
		values[i] = i;

	// Initialization of the tree map
	TreeMap_init(&tree);

	// Fill the tree map
	for(int i = 0; i < TREE_MAP_VALUE_COUNT; ++i)
		TreeMap_insert(&tree, values + i);

	// Unfill the tree map
	for(int i = 0; i < TREE_MAP_VALUE_COUNT; ++i) {
		// Check that all inserted elements can be found
		for(int j = i; j < TREE_MAP_VALUE_COUNT; ++j) {
			TreeMapNode* node = TreeMap_find(&tree, values + i);
			if (!node)
				mu_fail("TreeMapNode should be found");

			mu_check(node->key == values + i);
		}

		// Remove the element
		TreeMapNode* node = TreeMap_find(&tree, values + i);
		TreeMap_erase(&tree, node);

		// Check that all erase elements can be found
		for(int j = 0; j <= i; ++j) {
			TreeMapNode* node = TreeMap_find(&tree, values + i);
			if (node)
				mu_fail("TreeMapNode should not be found");
		}
	}

	// Release ressources
	TreeMap_destroy(&tree);
	free(values);
}


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

MU_TEST_SUITE(test_TreeMap_suite) {
	MU_RUN_TEST(test_TreeMap_insert);
	MU_RUN_TEST(test_TreeMap_erase);
}


MU_TEST_SUITE(test_StringList_suite) {
	MU_RUN_TEST(test_StringList_join);
}


int
main(
	ATTRIBUTE_UNUSED int argc,
	ATTRIBUTE_UNUSED char *argv[]
) {
	MU_RUN_SUITE(test_TreeMap_suite);
	MU_RUN_SUITE(test_StringList_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
