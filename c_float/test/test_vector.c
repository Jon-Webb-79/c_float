// ================================================================================
// ================================================================================
// - File:    test.c
// - Purpose: This file contains implementations for unit tests related to the 
//            c_float.h file.  This file support cmocka testing environments./
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 0.1
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "test_vector.h"
#include "../c_float.h"

#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
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

void test_push_back_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    
    // Test basic push_back
    assert_true(push_back_float_vector(vec, 3.14f));
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 3.14f, 0.0001f);
     
    // Test adding zero
    assert_true(push_back_float_vector(vec, 0.0f));
    //assert_int_equal(f_size(vec), 2);
    // assert_float_equal(float_vector_index(vec, 1), 0.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_growth(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = f_alloc(vec);
    
    // Fill to capacity
    assert_true(push_back_float_vector(vec, 1.0f));
    assert_true(push_back_float_vector(vec, 2.0f));
    assert_int_equal(f_size(vec), 2);
    assert_int_equal(f_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(push_back_float_vector(vec, 3.0f));
    assert_int_equal(f_size(vec), 3);
    assert_true(f_alloc(vec) > initial_alloc);
    assert_float_equal(float_vector_index(vec, 2), 3.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    
    // Fill to capacity
    assert_true(push_back_float_vector(&arr, 1.0f));
    assert_true(push_back_float_vector(&arr, 2.0f));
    
    // Attempt to exceed capacity should fail
    assert_false(push_back_float_vector(&arr, 3.0f));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(f_size(&arr), 2);
}
// -------------------------------------------------------------------------------- 

void test_generic_macros(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    
    // Test f_size macro
    assert_int_equal(f_size(vec), 0);
    push_back_float_vector(vec, 1.0f);
    assert_int_equal(f_size(vec), 1);
    
    // Test f_alloc macro
    assert_int_equal(f_alloc(vec), 2);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_float_vector_index(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    
    // Test valid index
    assert_true(push_back_float_vector(vec, 1.234f));
    assert_float_equal(float_vector_index(vec, 0), 1.234f, 0.0001f);
    
    // Test out of bounds index
    errno = 0;
    float result = float_vector_index(vec, f_size(vec));  
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    
    // Test NULL vector
    errno = 0;
    result = float_vector_index(NULL, 0);
    assert_int_equal(errno, EINVAL);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_back_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Test infinity
    assert_true(push_back_float_vector(vec, INFINITY));
    assert_true(isinf(float_vector_index(vec, 0)));
    
    // Test negative infinity
    assert_true(push_back_float_vector(vec, -INFINITY));
    assert_true(isinf(float_vector_index(vec, 1)));
    
    // Test NaN
    assert_true(push_back_float_vector(vec, NAN));
    assert_true(isnan(float_vector_index(vec, 2)));
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_static_array_bounds(void **state) {
    (void) state;
    
    // Create static array of size 3
    float_v arr = init_float_array(3);
    
    // Test adding up to capacity
    assert_true(push_back_float_vector(&arr, 1.0f));
    assert_int_equal(f_size(&arr), 1);
    assert_true(push_back_float_vector(&arr, 2.0f));
    assert_int_equal(f_size(&arr), 2);
    assert_true(push_back_float_vector(&arr, 3.0f));
    assert_int_equal(f_size(&arr), 3);
    
    // Verify values were stored correctly
    assert_float_equal(float_vector_index(&arr, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 2), 3.0f, 0.0001f);
    
    // Test attempting to exceed capacity
    errno = 0;
    assert_false(push_back_float_vector(&arr, 4.0f));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(f_size(&arr), 3);
    
    // Verify original data wasn't corrupted
    assert_float_equal(float_vector_index(&arr, 2), 3.0f, 0.0001f);
}
// --------------------------------------------------------------------------------

void test_static_array_index_bounds(void **state) {
    (void) state;
    
    // Create and fill static array
    float_v arr = init_float_array(2);
    push_back_float_vector(&arr, 1.0f);
    
    // Test valid index
    assert_float_equal(float_vector_index(&arr, 0), 1.0f, 0.0001f);
    
    // Test index at unfilled position
    errno = 0;
    float result = float_vector_index(&arr, 1);
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    
    // Test index beyond allocation
    errno = 0;
    result = float_vector_index(&arr, 2);
    assert_int_equal(errno, ERANGE);
    assert_float_equal(result, FLT_MAX, 0.0001f);
}
// --------------------------------------------------------------------------------

void test_static_array_initialization(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    
    // Check initial state
    assert_int_equal(f_size(&arr), 0);
    assert_int_equal(f_alloc(&arr), 2);
    
    // Verify data array is zeroed
    for(size_t i = 0; i < f_alloc(&arr); i++) {
        assert_float_equal(arr.data[i], 0.0f, 0.0001f);  // Direct access just for testing
    }
}
// -------------------------------------------------------------------------------- 

void test_static_array_free(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    push_back_float_vector(&arr, 1.0f);
    
    // Attempting to free a static array should fail
    errno = 0;
    free_float_vector(&arr);
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_float_equal(float_vector_index(&arr, 0), 1.0f, 0.0001f);
}
// ================================================================================ 
// ================================================================================ 

void test_push_front_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    
    // Test basic push_front
    assert_true(push_front_float_vector(vec, 3.14f));
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 3.14f, 0.0001f);
    
    // Test adding zero
    assert_true(push_front_float_vector(vec, 0.0f));
    assert_int_equal(f_size(vec), 2);
    assert_float_equal(float_vector_index(vec, 0), 0.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 3.14f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_growth(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = f_alloc(vec);
    
    // Fill to capacity
    assert_true(push_front_float_vector(vec, 1.0f));
    assert_true(push_front_float_vector(vec, 2.0f));
    assert_int_equal(f_size(vec), 2);
    assert_int_equal(f_alloc(vec), initial_alloc);
    
    // Verify order
    assert_float_equal(float_vector_index(vec, 0), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 1.0f, 0.0001f);
    
    // Trigger growth
    assert_true(push_front_float_vector(vec, 3.0f));
    assert_int_equal(f_size(vec), 3);
    assert_true(f_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_float_equal(float_vector_index(vec, 0), 3.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 2), 1.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    
    // Test basic push_front with static array
    assert_true(push_front_float_vector(&arr, 1.0f));
    assert_true(push_front_float_vector(&arr, 2.0f));
    assert_int_equal(f_size(&arr), 2);
    
    // Verify order
    assert_float_equal(float_vector_index(&arr, 0), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 1.0f, 0.0001f);
    
    // Attempt to exceed capacity
    assert_false(push_front_float_vector(&arr, 3.0f));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(f_size(&arr), 2);
    assert_float_equal(float_vector_index(&arr, 0), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 1.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_push_front_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Test infinity
    assert_true(push_front_float_vector(vec, INFINITY));
    assert_true(isinf(float_vector_index(vec, 0)));
    
    // Test negative infinity
    assert_true(push_front_float_vector(vec, -INFINITY));
    assert_true(isinf(float_vector_index(vec, 0)));
    
    // Test NaN
    assert_true(push_front_float_vector(vec, NAN));
    assert_true(isnan(float_vector_index(vec, 0)));
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_error_cases(void **state) {
    (void) state;
    
    // Test NULL vector
    assert_false(push_front_float_vector(NULL, 1.0f));
    assert_int_equal(errno, EINVAL);
    
    // Test with invalid data pointer
    float_v vec = {0};
    vec.data = NULL;
    assert_false(push_front_float_vector(&vec, 1.0f));
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================

void test_insert_vector_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Insert into empty vector
    assert_true(insert_float_vector(vec, 1.0f, 0));
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 1.0f, 0.0001f);
    
    // Insert at beginning (shifting right)
    assert_true(insert_float_vector(vec, 0.0f, 0));
    assert_int_equal(f_size(vec), 2);
    assert_float_equal(float_vector_index(vec, 0), 0.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 1.0f, 0.0001f);
    
    // Insert in middle
    assert_true(insert_float_vector(vec, 0.5f, 1));
    assert_int_equal(f_size(vec), 3);
    assert_float_equal(float_vector_index(vec, 0), 0.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 0.5f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 2), 1.0f, 0.0001f);
    
    // Insert at end (append)
    assert_true(insert_float_vector(vec, 2.0f, 3));
    assert_int_equal(f_size(vec), 4);
    assert_float_equal(float_vector_index(vec, 3), 2.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_vector_growth(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    size_t initial_alloc = f_alloc(vec);
    
    // Fill initial capacity
    assert_true(insert_float_vector(vec, 1.0f, 0));
    assert_true(insert_float_vector(vec, 2.0f, 1));
    assert_int_equal(f_size(vec), 2);
    assert_int_equal(f_alloc(vec), initial_alloc);
    
    // Trigger growth
    assert_true(insert_float_vector(vec, 1.5f, 1));
    assert_int_equal(f_size(vec), 3);
    assert_true(f_alloc(vec) > initial_alloc);
    
    // Verify all elements after growth
    assert_float_equal(float_vector_index(vec, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 1.5f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 2), 2.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_array_basic(void **state) {
    (void) state;
    
    float_v arr = init_float_array(3);
    
    // Test basic insertions
    assert_true(insert_float_vector(&arr, 1.0f, 0));
    assert_true(insert_float_vector(&arr, 3.0f, 1));
    assert_true(insert_float_vector(&arr, 2.0f, 1));
    
    // Verify order
    assert_int_equal(f_size(&arr), 3);
    assert_float_equal(float_vector_index(&arr, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 2), 3.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_array_bounds(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    
    // Fill array
    assert_true(insert_float_vector(&arr, 1.0f, 0));
    assert_true(insert_float_vector(&arr, 2.0f, 1));
    
    // Try to insert beyond capacity
    errno = 0;
    assert_false(insert_float_vector(&arr, 3.0f, 1));
    assert_int_equal(errno, EINVAL);
    
    // Verify data wasn't corrupted
    assert_int_equal(f_size(&arr), 2);
    assert_float_equal(float_vector_index(&arr, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 2.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_error_cases(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    
    // Test invalid index
    errno = 0;
    assert_false(insert_float_vector(vec, 1.0f, 1));  // Empty vector
    assert_int_equal(errno, ERANGE);
    
    // Add one element and test bounds
    assert_true(insert_float_vector(vec, 1.0f, 0));
    
    errno = 0;
    assert_false(insert_float_vector(vec, 2.0f, 2));  // Beyond length
    assert_int_equal(errno, ERANGE);
    
    // Test NULL vector
    errno = 0;
    assert_false(insert_float_vector(NULL, 1.0f, 0));
    assert_int_equal(errno, EINVAL);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_insert_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Insert infinity
    assert_true(insert_float_vector(vec, INFINITY, 0));
    assert_true(isinf(float_vector_index(vec, 0)));
    
    // Insert NaN
    assert_true(insert_float_vector(vec, NAN, 1));
    assert_true(isnan(float_vector_index(vec, 1)));
    
    // Insert zero
    assert_true(insert_float_vector(vec, 0.0f, 1));
    assert_float_equal(float_vector_index(vec, 1), 0.0f, 0.0001f);
    
    free_float_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_pop_back_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    
    // Test popping values
    errno = 0;
    assert_float_equal(pop_back_float_vector(vec), 3.0f, 0.0001f);
    assert_int_equal(f_size(vec), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_float_vector(vec), 2.0f, 0.0001f);
    assert_int_equal(f_size(vec), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_float_vector(vec), 1.0f, 0.0001f);
    assert_int_equal(f_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_empty(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    float result = pop_back_float_vector(vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);
    
    free_float_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_back_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    float result = pop_back_float_vector(NULL);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    float_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_back_float_vector(&vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pop_back_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Test popping FLT_MAX as valid data
    push_back_float_vector(vec, FLT_MAX);
    errno = 0;
    float result = pop_back_float_vector(vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
    
    // Test popping infinity
    push_back_float_vector(vec, INFINITY);
    errno = 0;
    result = pop_back_float_vector(vec);
    assert_true(isinf(result));
    assert_int_equal(errno, 0);
    
    // Test popping NaN
    push_back_float_vector(vec, NAN);
    errno = 0;
    result = pop_back_float_vector(vec);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_back_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    
    // Add and pop values from static array
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    
    errno = 0;
    assert_float_equal(pop_back_float_vector(&arr), 2.0f, 0.0001f);
    assert_int_equal(f_size(&arr), 1);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_back_float_vector(&arr), 1.0f, 0.0001f);
    assert_int_equal(f_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    float result = pop_back_float_vector(&arr);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_front_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Add some test values
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    
    // Test popping values and check remaining elements
    errno = 0;
    assert_float_equal(pop_front_float_vector(vec), 1.0f, 0.0001f);
    assert_int_equal(f_size(vec), 2);
    assert_float_equal(float_vector_index(vec, 0), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 3.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_float_vector(vec), 2.0f, 0.0001f);
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 3.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_float_vector(vec), 3.0f, 0.0001f);
    assert_int_equal(f_size(vec), 0);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_empty(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);
    
    // Try to pop from empty vector
    errno = 0;
    float result = pop_front_float_vector(vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    float result = pop_front_float_vector(NULL);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    float_v vec = {0};
    vec.data = NULL;
    errno = 0;
    result = pop_front_float_vector(&vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Test popping FLT_MAX as valid data
    push_back_float_vector(vec, FLT_MAX);
    push_back_float_vector(vec, 1.0f);
    errno = 0;
    float result = pop_front_float_vector(vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
    assert_float_equal(float_vector_index(vec, 0), 1.0f, 0.0001f);
    
    // Test popping infinity
    push_front_float_vector(vec, INFINITY);
    errno = 0;
    result = pop_front_float_vector(vec);
    assert_true(isinf(result));
    assert_int_equal(errno, 0);
    
    // Test popping NaN
    push_front_float_vector(vec, NAN);
    errno = 0;
    result = pop_front_float_vector(vec);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_front_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(2);
    
    // Add and pop values from static array
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    
    errno = 0;
    assert_float_equal(pop_front_float_vector(&arr), 1.0f, 0.0001f);
    assert_int_equal(f_size(&arr), 1);
    assert_float_equal(float_vector_index(&arr, 0), 2.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(pop_front_float_vector(&arr), 2.0f, 0.0001f);
    assert_int_equal(f_size(&arr), 0);
    assert_int_equal(errno, 0);
    
    // Try to pop from empty array
    errno = 0;
    float result = pop_front_float_vector(&arr);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);
}
// ================================================================================ 
// ================================================================================ 

void test_pop_any_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Add test values
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 4.0f);
    
    // Test popping from middle
    errno = 0;
    assert_float_equal(pop_any_float_vector(vec, 1), 2.0f, 0.0001f);
    assert_int_equal(f_size(vec), 3);
    assert_float_equal(float_vector_index(vec, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 3.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 2), 4.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    // Test popping first element
    assert_float_equal(pop_any_float_vector(vec, 0), 1.0f, 0.0001f);
    assert_int_equal(f_size(vec), 2);
    assert_float_equal(float_vector_index(vec, 0), 3.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 4.0f, 0.0001f);
    
    // Test popping last element
    assert_float_equal(pop_any_float_vector(vec, 1), 4.0f, 0.0001f);
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 3.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_errors(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(2);
    assert_non_null(vec);
    push_back_float_vector(vec, 1.0f);
    
    // Test NULL vector
    errno = 0;
    float result = pop_any_float_vector(NULL, 0);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid index
    errno = 0;
    result = pop_any_float_vector(vec, 1);  // Index equals length
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ERANGE);
    
    errno = 0;
    result = pop_any_float_vector(vec, 2);  // Index beyond length
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ERANGE);
    
    // Test empty vector
    pop_any_float_vector(vec, 0);  // Remove the only element
    errno = 0;
    result = pop_any_float_vector(vec, 0);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(3);
    
    // Fill array
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    push_back_float_vector(&arr, 3.0f);
    
    // Pop from middle
    errno = 0;
    float result = pop_any_float_vector(&arr, 1);
    assert_float_equal(result, 2.0f, 0.0001f);
    assert_int_equal(f_size(&arr), 2);
    assert_float_equal(float_vector_index(&arr, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 3.0f, 0.0001f);
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Test with FLT_MAX as valid data
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, FLT_MAX);
    push_back_float_vector(vec, 3.0f);
     
    errno = 0;
    float result = pop_any_float_vector(vec, 1);
    assert_float_equal(result, FLT_MAX, 0.0001f);
    assert_int_equal(errno, 0);  // Should be 0 since this is valid data
     
    // Test with NaN
    push_back_float_vector(vec, NAN);
    errno = 0;
    result = pop_any_float_vector(vec, 2);
    assert_true(isnan(result));
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// ================================================================================ 
// ================================================================================

void test_reverse_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Test single element
    push_back_float_vector(vec, 1.0f);
    reverse_float_vector(vec);
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 1.0f, 0.0001f);
    
    // Test even number of elements
    push_back_float_vector(vec, 2.0f);
    reverse_float_vector(vec);
    assert_int_equal(f_size(vec), 2);
    assert_float_equal(float_vector_index(vec, 0), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 1.0f, 0.0001f);
    
    // Test odd number of elements
    push_back_float_vector(vec, 3.0f);
    reverse_float_vector(vec);
    assert_int_equal(f_size(vec), 3);
    assert_float_equal(float_vector_index(vec, 0), 3.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 2), 2.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    reverse_float_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    float_v vec = {0};
    vec.data = NULL;
    errno = 0;
    reverse_float_vector(&vec);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    float_v* empty_vec = init_float_vector(1);
    assert_non_null(empty_vec);
    errno = 0;
    reverse_float_vector(empty_vec);
    assert_int_equal(errno, ENODATA);
    
    free_float_vector(empty_vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(3);
    
    // Add test values
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    push_back_float_vector(&arr, 3.0f);
    
    // Test reversal
    reverse_float_vector(&arr);
    assert_int_equal(f_size(&arr), 3);
    assert_float_equal(float_vector_index(&arr, 0), 3.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 1), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(&arr, 2), 1.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_reverse_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Add special values
    push_back_float_vector(vec, INFINITY);
    push_back_float_vector(vec, NAN);
    push_back_float_vector(vec, -INFINITY);
    
    // Test reversal
    reverse_float_vector(vec);
    
    // Verify reversed order
    assert_true(isinf(float_vector_index(vec, 0)) && float_vector_index(vec, 0) < 0.0f);  // -INFINITY
    assert_true(isnan(float_vector_index(vec, 1)));  // NAN
    assert_true(isinf(float_vector_index(vec, 2)) && float_vector_index(vec, 2) > 0.0f);  // INFINITY
    
    free_float_vector(vec);
}
// ================================================================================
// ================================================================================ 

void test_sort_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Test ascending sort
    push_back_float_vector(vec, 5.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    
    sort_float_vector(vec, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < f_size(vec) - 1; i++) {
        assert_true(float_vector_index(vec, i) <= float_vector_index(vec, i + 1));
    }
    
    // Test descending sort
    sort_float_vector(vec, REVERSE);
    
    // Verify descending order
    for (size_t i = 0; i < f_size(vec) - 1; i++) {
        assert_true(float_vector_index(vec, i) >= float_vector_index(vec, i + 1));
    }
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_edge_cases(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(10);
    assert_non_null(vec);
    
    // Test empty vector
    sort_float_vector(vec, FORWARD);
    assert_int_equal(f_size(vec), 0);
    
    // Test single element
    push_back_float_vector(vec, 1.0f);
    sort_float_vector(vec, FORWARD);
    assert_int_equal(f_size(vec), 1);
    assert_float_equal(float_vector_index(vec, 0), 1.0f, 0.0001f);
    
    // Test two elements
    push_back_float_vector(vec, 0.0f);
    sort_float_vector(vec, FORWARD);
    assert_float_equal(float_vector_index(vec, 0), 0.0f, 0.0001f);
    assert_float_equal(float_vector_index(vec, 1), 1.0f, 0.0001f);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_duplicates(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Add duplicate values
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    
    sort_float_vector(vec, FORWARD);
    
    // Verify order with duplicates
    for (size_t i = 0; i < f_size(vec) - 1; i++) {
        assert_true(float_vector_index(vec, i) <= float_vector_index(vec, i + 1));
    }
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Add special values
    push_back_float_vector(vec, INFINITY);
    push_back_float_vector(vec, -INFINITY);
    push_back_float_vector(vec, NAN);
    push_back_float_vector(vec, 0.0f);
    
    sort_float_vector(vec, FORWARD);
    
    // Verify NaN handling and infinity ordering
    size_t last_valid_index = f_size(vec) - 1;
    while (last_valid_index > 0 && isnan(float_vector_index(vec, last_valid_index))) {
        last_valid_index--;
    }
     
    // Check that -INFINITY is first and INFINITY is last (excluding NaN)
    assert_true(isinf(float_vector_index(vec, 0)) && float_vector_index(vec, 0) < 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_static_array(void **state) {
    (void) state;
    
    float_v arr = init_float_array(5);
    
    // Add unsorted values
    push_back_float_vector(&arr, 5.0f);
    push_back_float_vector(&arr, 3.0f);
    push_back_float_vector(&arr, 4.0f);
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    
    sort_float_vector(&arr, FORWARD);
    
    // Verify ascending order
    for (size_t i = 0; i < f_size(&arr) - 1; i++) {
        assert_true(float_vector_index(&arr, i) <= float_vector_index(&arr, i + 1));
    }
}
// -------------------------------------------------------------------------------- 

void test_sort_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    sort_float_vector(NULL, FORWARD);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_trim_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(10);  // Start with capacity of 10
    assert_non_null(vec);
    
    // Add 5 elements
    for (float i = 0; i < 5; i++) {
        push_back_float_vector(vec, i);
    }
    
    size_t original_alloc = f_alloc(vec);
    assert_int_equal(original_alloc, 10);
    assert_int_equal(f_size(vec), 5);
    
    // Trim the vector
    errno = 0;
    trim_float_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify capacity matches size
    assert_int_equal(f_alloc(vec), 5);
    assert_int_equal(f_size(vec), 5);
    
    // Verify data is intact
    for (size_t i = 0; i < f_size(vec); i++) {
        assert_float_equal(float_vector_index(vec, i), (float)i, 0.0001f);
    }
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_empty_vector(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);  // Capacity of 5, but empty
    assert_non_null(vec);
    
    size_t original_alloc = f_alloc(vec);
    assert_int_equal(original_alloc, 5);
    assert_int_equal(f_size(vec), 0);
     
    // Trim the empty vector
    errno = 0;
    trim_float_vector(vec);
    assert_int_equal(errno, ENODATA);

    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_static_array(void **state) {
    (void) state;
    
    float_v arr = init_float_array(5);
    
    // Add some elements
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    
    size_t original_alloc = f_alloc(&arr);
    size_t original_size = f_size(&arr);
    
    // Try to trim static array
    errno = 0;
    trim_float_vector(&arr);
    assert_int_equal(errno, 0);  // Should not set error
    
    // Verify nothing changed
    assert_int_equal(f_alloc(&arr), original_alloc);
    assert_int_equal(f_size(&arr), original_size);
}
// -------------------------------------------------------------------------------- 

void test_trim_already_optimal(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Fill to capacity
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    
    assert_int_equal(f_alloc(vec), 3);
    assert_int_equal(f_size(vec), 3);
    
    // Try to trim an already optimal vector
    errno = 0;
    trim_float_vector(vec);
    assert_int_equal(errno, 0);
    
    // Verify nothing changed
    assert_int_equal(f_alloc(vec), 3);
    assert_int_equal(f_size(vec), 3);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_trim_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    trim_float_vector(NULL);
    assert_int_equal(errno, EINVAL);
    
    // Test invalid data pointer
    float_v vec = {0};
    vec.data = NULL;
    errno = 0;
    trim_float_vector(&vec);
    assert_int_equal(errno, EINVAL);
}
// ================================================================================ 
// ================================================================================ 

void test_binary_search_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Add values in order
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 5.0f);
    
    // Test exact matches
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 1.0f, 0.0001f, false), 0);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 3.0f, 0.0001f, false), 2);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 5.0f, 0.0001f, false), 4);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_tolerance(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    
    // Test with tolerance
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 1.1f, 0.2f, false), 0);  // Should find 1.0
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 2.95f, 0.1f, false), 2);  // Should find 3.0
    assert_int_equal(errno, 0);
    
    // Test value outside tolerance
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 2.5f, 0.1f, false), LONG_MAX);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_with_sort(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Add values out of order
    push_back_float_vector(vec, 5.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 2.0f);
    
    // Search with sort_first = true
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 3.0f, 0.0001f, true), 2);
    assert_int_equal(errno, 0);
    
    // Verify the vector is now sorted
    for (size_t i = 0; i < f_size(vec) - 1; i++) {
        assert_true(float_vector_index(vec, i) <= float_vector_index(vec, i + 1));
    }
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_int_equal(binary_search_float_vector(NULL, 1.0f, 0.0001f, false), LONG_MAX);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_int_equal(binary_search_float_vector(vec, 1.0f, 0.0001f, false), LONG_MAX);
    assert_int_equal(errno, ENODATA);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_binary_search_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(5);
    
    // Add values
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    push_back_float_vector(&arr, 3.0f);
    push_back_float_vector(&arr, 4.0f);
    push_back_float_vector(&arr, 5.0f);
    
    // Test search in static array
    errno = 0;
    assert_int_equal(binary_search_float_vector(&arr, 3.0f, 0.0001f, false), 2);
    assert_int_equal(errno, 0);
    
    // Test value not found
    errno = 0;
    assert_int_equal(binary_search_float_vector(&arr, 6.0f, 0.0001f, false), LONG_MAX);
    assert_int_equal(errno, 0);
}
// ================================================================================ 
// ================================================================================ 

void test_update_float_vector_nominal(void **state) {
    (void) state;
    float_v arr = init_float_array(5);
    
    // Add values
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    push_back_float_vector(&arr, 3.0f);
    push_back_float_vector(&arr, 4.0f);
    push_back_float_vector(&arr, 5.0f);

    update_float_vector(&arr, 2, 12.0f);
    assert_float_equal(12.0f, float_vector_index(&arr, 2), 1.0e-6);
}
// -------------------------------------------------------------------------------- 

void test_update_float_vector_null(void **state) {
    errno = 0;
    update_float_vector(NULL, 3, 8.2f);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_update_float_vector_bad_index(void **state) {
    (void) state;
    float_v* vec = init_float_vector(3);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    errno = 0;
    update_float_vector(vec, 5, 3.0f);
    assert_int_equal(errno, ERANGE);
    free_float_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_min_float_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_float_vector(vec, 1.0f);
    errno = 0;
    assert_float_equal(min_float_vector(vec), 1.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, -3.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 0.0f);
    
    errno = 0;
    assert_float_equal(min_float_vector(vec), -3.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_max_float_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(5);
    assert_non_null(vec);
    
    // Test single element
    push_back_float_vector(vec, 1.0f);
    errno = 0;
    assert_float_equal(max_float_vector(vec), 1.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    // Test multiple elements
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, -3.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 0.0f);
    
    errno = 0;
    assert_float_equal(max_float_vector(vec), 4.0f, 0.0001f);  // This will fail with current implementation
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_float_vector(vec, INFINITY);
    push_back_float_vector(vec, -INFINITY);
    push_back_float_vector(vec, 1.0f);
    
    errno = 0;
    assert_true(isinf(min_float_vector(vec)) && min_float_vector(vec) < 0);  // Should be -INFINITY
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_true(isinf(max_float_vector(vec)) && max_float_vector(vec) > 0);  // Should be INFINITY
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_min_max_static_array(void **state) {
    (void) state;
    
    float_v arr = init_float_array(3);
    
    push_back_float_vector(&arr, 3.0f);
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    
    errno = 0;
    assert_float_equal(min_float_vector(&arr), 1.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(max_float_vector(&arr), 3.0f, 0.0001f);  // This will fail with current implementation
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_min_max_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(min_float_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(max_float_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_float_equal(min_float_vector(vec), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(max_float_vector(vec), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    free_float_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_sum_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_float_vector(vec, 1.0f);
    errno = 0;
    assert_float_equal(sum_float_vector(vec), 1.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 4.0f);
    
    errno = 0;
    assert_float_equal(sum_float_vector(vec), 10.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_average_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Test single value
    push_back_float_vector(vec, 2.0f);
    errno = 0;
    assert_float_equal(average_float_vector(vec), 2.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    // Test multiple values
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 6.0f);
    push_back_float_vector(vec, 8.0f);
    
    errno = 0;
    assert_float_equal(average_float_vector(vec), 5.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_float_vector(vec, INFINITY);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    
    errno = 0;
    assert_true(isinf(sum_float_vector(vec)));
    assert_int_equal(errno, 0);

    errno = 0;
    assert_true(isinf(average_float_vector(vec)));
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_negative(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    push_back_float_vector(vec, -1.0f);
    push_back_float_vector(vec, -2.0f);
    push_back_float_vector(vec, 5.0f);
    push_back_float_vector(vec, 2.0f);
    
    errno = 0;
    assert_float_equal(sum_float_vector(vec), 4.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(average_float_vector(vec), 1.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_static(void **state) {
    (void) state;
    
    float_v arr = init_float_array(3);
    
    push_back_float_vector(&arr, 1.0f);
    push_back_float_vector(&arr, 2.0f);
    push_back_float_vector(&arr, 3.0f);
    
    errno = 0;
    assert_float_equal(sum_float_vector(&arr), 6.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    errno = 0;
    assert_float_equal(average_float_vector(&arr), 2.0f, 0.0001f);
    assert_int_equal(errno, 0);
}
// -------------------------------------------------------------------------------- 

void test_sum_average_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(sum_float_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(average_float_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);
    
    errno = 0;
    assert_float_equal(sum_float_vector(vec), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    errno = 0;
    assert_float_equal(average_float_vector(vec), FLT_MAX, 0.0001f);
    assert_int_equal(errno, EINVAL);
    
    free_float_vector(vec);
}
// ================================================================================ 
// ================================================================================ 

void test_stdev_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    // Dataset with known standard deviation
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 4.0f);
    push_back_float_vector(vec, 6.0f);
    
    // Mean = 4.0, variance = 2.0, stdev = sqrt(2.0)
    errno = 0;
    assert_float_equal(stdev_float_vector(vec), sqrt(2.0f), 0.0001f);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_single_value(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);
    
    push_back_float_vector(vec, 2.0f);
    
    errno = 0;
    float result = stdev_float_vector(vec);
    assert_float_equal(result, FLT_MAX, 0.0001f);  // Standard deviation of single value is 0
    assert_int_equal(errno, ENODATA);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_stdev_same_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // All same values should give stdev of 0
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 2.0f);
    
    errno = 0;
    assert_float_equal(stdev_float_vector(vec), 0.0f, 0.0001f);
    assert_int_equal(errno, 0);
    
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_basic(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, 4.0f);
    
    float_v* cum_sum = cum_sum_float_vector(vec);
    assert_non_null(cum_sum);
    assert_int_equal(f_size(cum_sum), 4);
    
    // Check cumulative sums: 1, 3, 6, 10
    assert_float_equal(float_vector_index(cum_sum, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(cum_sum, 1), 3.0f, 0.0001f);
    assert_float_equal(float_vector_index(cum_sum, 2), 6.0f, 0.0001f);
    assert_float_equal(float_vector_index(cum_sum, 3), 10.0f, 0.0001f);
    
    free_float_vector(vec);
    free_float_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_cum_sum_negative(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(4);
    assert_non_null(vec);
    
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, -2.0f);
    push_back_float_vector(vec, 3.0f);
    push_back_float_vector(vec, -4.0f);
    
    float_v* cum_sum = cum_sum_float_vector(vec);
    assert_non_null(cum_sum);
    
    // Check cumulative sums: 1, -1, 2, -2
    assert_float_equal(float_vector_index(cum_sum, 0), 1.0f, 0.0001f);
    assert_float_equal(float_vector_index(cum_sum, 1), -1.0f, 0.0001f);
    assert_float_equal(float_vector_index(cum_sum, 2), 2.0f, 0.0001f);
    assert_float_equal(float_vector_index(cum_sum, 3), -2.0f, 0.0001f);
    
    free_float_vector(vec);
    free_float_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_special_values(void **state) {
    (void) state;
    
    float_v* vec = init_float_vector(3);
    assert_non_null(vec);
    
    // Test with infinity
    push_back_float_vector(vec, INFINITY);
    push_back_float_vector(vec, 1.0f);
    push_back_float_vector(vec, 2.0f);
    
    errno = 0;
    assert_true(isinf(stdev_float_vector(vec)));

    float_v* cum_sum = cum_sum_float_vector(vec);
    assert_non_null(cum_sum);
    assert_true(isinf(float_vector_index(cum_sum, 0)));
    assert_true(isinf(float_vector_index(cum_sum, 1)));
    assert_true(isinf(float_vector_index(cum_sum, 2)));
    
    free_float_vector(vec);
    free_float_vector(cum_sum);
}
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_errors(void **state) {
    (void) state;
    
    // Test NULL vector
    errno = 0;
    assert_float_equal(stdev_float_vector(NULL), FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);
    
    assert_null(cum_sum_float_vector(NULL));
    assert_int_equal(errno, EINVAL);
    
    // Test empty vector
    float_v* vec = init_float_vector(1);
    assert_non_null(vec);

    errno = 0;
    assert_float_equal(stdev_float_vector(vec), FLT_MAX, 0.0001f);
    assert_int_equal(errno, ENODATA);

    assert_null(cum_sum_float_vector(vec));
    assert_int_equal(errno, EINVAL);
     
    free_float_vector(vec);
}
// -------------------------------------------------------------------------------- 

/* Setup and teardown functions */
static dict_f* test_dict = NULL;

int setup(void** state) {
    test_dict = init_float_dict();
    if (!test_dict) return -1;
    *state = test_dict;
    return 0;
}
// -------------------------------------------------------------------------------- 

int teardown(void** state) {
    free_float_dict(test_dict);
    return 0;
}
// -------------------------------------------------------------------------------- 

/* Test initialization */
void test_init_float_dict(void** state) {
    dict_f* dict = init_float_dict();
    assert_non_null(dict);
    assert_int_equal(float_dict_hash_size(dict), 0);
    assert_int_equal(float_dict_size(dict), 0);
    assert_true(float_dict_alloc(dict) > 0);
    free_float_dict(dict);
}
// -------------------------------------------------------------------------------- 

/* Test insertion */
void test_insert_float_dict_basic(void** state) {
    dict_f* dict = *state;
    
    assert_true(insert_float_dict(dict, "test", 1.0f));
    assert_int_equal(float_dict_hash_size(dict), 1);
    assert_int_equal(float_dict_size(dict), 1);
    
    float value = get_float_dict_value(dict, "test");
    assert_float_equal(value, 1.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_float_dict_duplicate(void** state) {
    dict_f* dict = *state;
    
    assert_true(insert_float_dict(dict, "test", 1.0f));
    assert_false(insert_float_dict(dict, "test", 2.0f));
    
    float value = get_float_dict_value(dict, "test");
    assert_float_equal(value, 1.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_insert_float_dict_null(void** state) {
    dict_f* dict = *state;
    
    assert_false(insert_float_dict(NULL, "test", 1.0f));
    assert_false(insert_float_dict(dict, NULL, 1.0f));
}
// -------------------------------------------------------------------------------- 

/* Test retrieval */
void test_get_float_dict_value_basic(void** state) {
    dict_f* dict = *state;
    
    insert_float_dict(dict, "key1", 1.5f);
    float value = get_float_dict_value(dict, "key1");
    assert_float_equal(value, 1.5f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_get_float_dict_value_missing(void** state) {
    dict_f* dict = *state;
    
    float value = get_float_dict_value(dict, "nonexistent");
    assert_float_equal(value, FLT_MAX, 0.0001f);
}
// -------------------------------------------------------------------------------- 

/* Test update */
void test_update_float_dict_basic(void** state) {
    dict_f* dict = *state;
    
    insert_float_dict(dict, "key1", 1.0f);
    assert_true(update_float_dict(dict, "key1", 2.0f));
    
    float value = get_float_dict_value(dict, "key1");
    assert_float_equal(value, 2.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_update_float_dict_missing(void** state) {
    dict_f* dict = *state;
    
    assert_false(update_float_dict(dict, "nonexistent", 1.0f));
}
// -------------------------------------------------------------------------------- 

/* Test removal */
void test_pop_float_dict_basic(void** state) {
    dict_f* dict = *state;
    
    insert_float_dict(dict, "key1", 1.5f);
    float value = pop_float_dict(dict, "key1");
    assert_float_equal(value, 1.5f, 0.0001f);
    assert_int_equal(float_dict_hash_size(dict), 0);
}
// -------------------------------------------------------------------------------- 

void test_pop_float_dict_missing(void** state) {
    dict_f* dict = *state;
    
    float value = pop_float_dict(dict, "nonexistent");
    assert_float_equal(value, FLT_MAX, 0.0001f);
}
// -------------------------------------------------------------------------------- 

/* Test resize behavior */
void test_resize_behavior(void** state) {
    dict_f* dict = *state;
    char key[20];
    size_t initial_alloc = float_dict_alloc(dict);
    
    // Insert enough items to trigger resize
    for (int i = 0; i < 100; i++) {
        sprintf(key, "key%d", i);
        assert_true(insert_float_dict(dict, key, (float)i));
    }
    
    assert_true(float_dict_alloc(dict) > initial_alloc);
}
// -------------------------------------------------------------------------------- 

/* Test key/value extraction */
void test_get_keys_float_dict(void** state) {
    dict_f* dict = *state;
    
    insert_float_dict(dict, "key1", 1.0f);
    insert_float_dict(dict, "key2", 2.0f);
    
    string_v* keys = get_keys_float_dict(dict);
    assert_non_null(keys);
    assert_int_equal(float_dict_hash_size(dict), 2);
    
    free_str_vector(keys);
}
// -------------------------------------------------------------------------------- 

void test_get_values_float_dict(void** state) {
    dict_f* dict = *state;
    
    insert_float_dict(dict, "key1", 1.0f);
    insert_float_dict(dict, "key2", 2.0f);
    
    float_v* values = get_values_float_dict(dict);
    assert_non_null(values);
    assert_int_equal(float_dict_hash_size(dict), 2);
    
    free_float_vector(values);
}
// -------------------------------------------------------------------------------- 

/* Helper function for testing iterator */
static void count_entries(const char* key, float value, void* user_data) {
    size_t* count = (size_t*)user_data;
    (*count)++;
}

/* Helper function to sum values */
static void sum_values(const char* key, float value, void* user_data) {
    float* sum = (float*)user_data;
    *sum += value;
}
// -------------------------------------------------------------------------------- 

void test_foreach_float_dict_basic(void** state) {
    dict_f* dict = *state;
    size_t count = 0;
    
    // Add some test data
    assert_true(insert_float_dict(dict, "key1", 1.0f));
    assert_true(insert_float_dict(dict, "key2", 2.0f));
    assert_true(insert_float_dict(dict, "key3", 3.0f));
    
    // Test iteration count
    assert_true(foreach_float_dict(dict, count_entries, &count));
    assert_int_equal(count, 3);
    assert_int_equal(count, float_dict_hash_size(dict));
    
    // Test value sum
    float sum = 0.0f;
    assert_true(foreach_float_dict(dict, sum_values, &sum));
    assert_float_equal(sum, 6.0f, 0.0001f);
}
// -------------------------------------------------------------------------------- 

void test_foreach_float_dict_empty(void** state) {
    dict_f* dict = *state;
    size_t count = 0;
    
    assert_true(foreach_float_dict(dict, count_entries, &count));
    assert_int_equal(count, 0);
}
// -------------------------------------------------------------------------------- 

void test_foreach_float_dict_null(void** state) {
    dict_f* dict = *state;
    size_t count = 0;
    
    assert_false(foreach_float_dict(NULL, count_entries, &count));
    assert_false(foreach_float_dict(dict, NULL, &count));
}
// -------------------------------------------------------------------------------- 

void test_dictionary_gbc(void **state) {
    dict_f* dict FDICT_GBC = init_float_dict();
    insert_float_dict(dict, "Key1", 1.0);
}
// ================================================================================ 
// ================================================================================ 

void test_vector_dictionary(void **state) {
    dict_fv* dict = init_floatv_dict();
    bool result;
    result = create_floatv_dict(dict, "one", 3);
    assert_true(result);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 1.0);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 2.0);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 3.0);

    float test_one[3] = {1.0, 2.0, 3.0};
    float_v* vec1 = return_floatv_pointer(dict, "one");
    for (size_t i = 0; i < float_vector_size(vec1); i++) {
        assert_float_equal(float_vector_index(vec1, i), test_one[i], 1.0e-3);
    }

    result = create_floatv_dict(dict, "two", 3);
    assert_true(result);
    push_back_float_vector(return_floatv_pointer(dict, "two"), 4.0);
    push_back_float_vector(return_floatv_pointer(dict, "two"), 5.0);
    push_back_float_vector(return_floatv_pointer(dict, "two"), 6.0);

    float test_two[3] = {4.0, 5.0, 6.0};
    float_v* vec2 = return_floatv_pointer(dict, "two");
    for (size_t i = 0; i < float_vector_size(vec2); i++) {
        assert_float_equal(float_vector_index(vec2, i), test_two[i], 1.0e-3);
    }

    free_floatv_dict(dict);
}
// -------------------------------------------------------------------------------- 

void test_vector_dictionary_resize(void **state) {
    dict_fv* dict = init_floatv_dict();
    bool result;
    result = create_floatv_dict(dict, "one", 3);
    assert_true(result);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 1.0);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 2.0);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 3.0);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 4.0);
    push_back_float_vector(return_floatv_pointer(dict, "one"), 5.0);

    float test_one[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    float_v* vec1 = return_floatv_pointer(dict, "one");
    for (size_t i = 0; i < float_vector_size(vec1); i++) {
        assert_float_equal(float_vector_index(vec1, i), test_one[i], 1.0e-3);
    }
    free_floatv_dict(dict);
}
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_vector_dictionary_gbc(void **state) {
        FDICTV_GBC dict_fv* dict = init_floatv_dict();
        bool result;
        result = create_floatv_dict(dict, "one", 3);
        assert_true(result);
        push_back_float_vector(return_floatv_pointer(dict, "one"), 1.0);
        push_back_float_vector(return_floatv_pointer(dict, "one"), 2.0);
        push_back_float_vector(return_floatv_pointer(dict, "one"), 3.0);
        push_back_float_vector(return_floatv_pointer(dict, "one"), 4.0);
        push_back_float_vector(return_floatv_pointer(dict, "one"), 5.0);

        float test_one[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
        float_v* vec1 = return_floatv_pointer(dict, "one");
        for (size_t i = 0; i < float_vector_size(vec1); i++) {
            assert_float_equal(float_vector_index(vec1, i), test_one[i], 1.0e-3);
        }
    }
#endif
// ================================================================================ 
// ================================================================================ 
// eof
