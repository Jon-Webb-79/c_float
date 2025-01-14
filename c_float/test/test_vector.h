// ================================================================================
// ================================================================================
// - File:    test.h
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

#ifndef test_flt_struct_H
#define test_flt_struct_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 

void test_init_float_vector_success(void **state);
// --------------------------------------------------------------------------------

void test_init_float_vector_zero_size(void **state);
// -------------------------------------------------------------------------------- 

void test_init_float_array_success(void **state);
// --------------------------------------------------------------------------------

void test_free_float_vector_null(void **state);
// -------------------------------------------------------------------------------- 

void test_free_float_vector_static(void **state);
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_float_vector_gbc(void **state);
#endif
// ================================================================================ 
// ================================================================================ 
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof