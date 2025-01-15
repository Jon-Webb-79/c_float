// ================================================================================
// ================================================================================
// - File:    unit_test.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "test_vector.h"

// Begin code
int main(int argc, const char * argv[]) {
	const struct CMUnitTest test_one[] = {
		cmocka_unit_test(test_init_float_vector_success),
        cmocka_unit_test(test_init_float_vector_zero_size),
        cmocka_unit_test(test_init_float_array_success),
        cmocka_unit_test(test_free_float_vector_null),
        cmocka_unit_test(test_free_float_vector_static),
        #if defined(__GNUC__) || defined(__clang__)
            cmocka_unit_test(test_float_vector_gbc),
        #endif
        cmocka_unit_test(test_push_back_basic),
        cmocka_unit_test(test_push_back_growth),
        cmocka_unit_test(test_push_back_static),
        cmocka_unit_test(test_generic_macros),
        cmocka_unit_test(test_float_vector_index),
        cmocka_unit_test(test_push_back_special_values),
        cmocka_unit_test(test_static_array_bounds),
        cmocka_unit_test(test_static_array_index_bounds),
        cmocka_unit_test(test_static_array_initialization),
        cmocka_unit_test(test_static_array_free),
        cmocka_unit_test(test_push_front_basic),
        cmocka_unit_test(test_push_front_growth),
        cmocka_unit_test(test_push_front_static),
        cmocka_unit_test(test_push_front_special_values),
        cmocka_unit_test(test_push_front_error_cases),
        cmocka_unit_test(test_insert_vector_basic),
        cmocka_unit_test(test_insert_vector_growth),
        cmocka_unit_test(test_insert_array_basic),
        cmocka_unit_test(test_insert_array_bounds),
        cmocka_unit_test(test_insert_error_cases),
        cmocka_unit_test(test_insert_special_values),
        cmocka_unit_test(test_pop_back_basic),
        cmocka_unit_test(test_pop_back_empty),
        cmocka_unit_test(test_pop_back_errors),
        cmocka_unit_test(test_pop_back_special_values),
        cmocka_unit_test(test_pop_back_static),
        cmocka_unit_test(test_pop_front_basic),
        cmocka_unit_test(test_pop_front_empty),
        cmocka_unit_test(test_pop_front_errors),
        cmocka_unit_test(test_pop_front_special_values),
        cmocka_unit_test(test_pop_front_static),
        cmocka_unit_test(test_pop_any_basic),
        cmocka_unit_test(test_pop_any_errors),
        cmocka_unit_test(test_pop_any_static),
        cmocka_unit_test(test_pop_any_special_values),
        cmocka_unit_test(test_reverse_basic),
        cmocka_unit_test(test_reverse_errors),
        cmocka_unit_test(test_reverse_static),
        cmocka_unit_test(test_reverse_special_values),
        cmocka_unit_test(test_sort_basic),
        cmocka_unit_test(test_sort_edge_cases),
        cmocka_unit_test(test_sort_duplicates),
        cmocka_unit_test(test_sort_special_values),
        cmocka_unit_test(test_sort_static_array),
        cmocka_unit_test(test_sort_errors)
	};
	return cmocka_run_group_tests(test_one, NULL, NULL);
}
// ================================================================================
// ================================================================================
// eof

