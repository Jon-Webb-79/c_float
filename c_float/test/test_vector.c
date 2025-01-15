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
#endif
// ================================================================================
// ================================================================================
// eof
