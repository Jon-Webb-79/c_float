// ================================================================================
// ================================================================================
// - File:    test.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "test_vector.h"
#include "../c_float.h"

#include <errno.h>
#include <stdlib.h>
// ================================================================================ 
// ================================================================================ 

void test_init_float_vector_success(void **state) {
    (void) state; /* unused */
    
    float_v* vec = init_float_vector(10);
    assert_non_null(vec);
    assert_non_null(vec->data);
    assert_int_equal(vec->len, 0);
    assert_int_equal(vec->alloc, 10);
    assert_int_equal(vec->alloc_type, DYNAMIC);
    
    // Check zero initialization
    for (size_t i = 0; i < vec->alloc; i++) {
        assert_float_equal(vec->data[i], 0.0f, 0.0001f);
    }
    
    free_float_vector(vec);
}
// --------------------------------------------------------------------------------

void test_init_float_vector_zero_size(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    float_v* vec = init_float_vector(0);
    assert_null(vec);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_init_float_array_success(void **state) {
    (void) state; /* unused */
    
    float_v arr = init_float_array(10);
    assert_non_null(arr.data);
    assert_int_equal(arr.len, 0);
    assert_int_equal(arr.alloc, 10);
    assert_int_equal(arr.alloc_type, STATIC);
    
    // Check zero initialization
    for (size_t i = 0; i < arr.alloc; i++) {
        assert_float_equal(arr.data[i], 0.0f, 0.0001f);
    }
}
// -------------------------------------------------------------------------------- 

void test_free_float_vector_null(void **state) {
    (void) state; /* unused */
    
    errno = 0;
    free_float_vector(NULL);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_free_float_vector_static(void **state) {
    (void) state; /* unused */
    
    float_v arr = init_float_array(10);
    float_v* arr_ptr = &arr;
    
    errno = 0;
    free_float_vector(arr_ptr);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
/* GC Tests */
void test_float_vector_gbc(void **state) {
    (void) state; /* unused */
    
    {
        FLTVEC_GBC float_v* vec = init_float_vector(10);
        assert_non_null(vec);
        assert_non_null(vec->data);
        // Vector will be automatically freed at scope end
    }
}
#endif
// ================================================================================
// ================================================================================
// eof
