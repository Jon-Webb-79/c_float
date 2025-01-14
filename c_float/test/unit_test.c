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
	};
	return cmocka_run_group_tests(test_one, NULL, NULL);
}
// ================================================================================
// ================================================================================
// eof

