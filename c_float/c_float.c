// ================================================================================
// ================================================================================
// - File:    c_float.c
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

#include "c_float.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>

//static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
//static const size_t hashSize = 3;  //  Size fo hash map initi functions
// ================================================================================
// ================================================================================ 

float_v* init_float_vector(size_t buff) {
    if (buff == 0) {
        errno = EINVAL;
        return NULL;
    }
    float_v* struct_ptr = malloc(sizeof(float_v));
    if (struct_ptr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
   
    float* data_ptr = malloc(buff * sizeof(float));
    if (data_ptr == NULL) {
        free(struct_ptr);
        errno = ENOMEM;
        return NULL; 
    }
   
    // Initialize all elements
    memset(data_ptr, 0, buff * sizeof(float));
   
    struct_ptr->data = data_ptr;
    struct_ptr->len = 0;
    struct_ptr->alloc = buff;
    struct_ptr->alloc_type = DYNAMIC;
    return struct_ptr;
}
// --------------------------------------------------------------------------------

void free_float_vector(float_v* vec) {
   if (!vec || !vec->alloc_type || vec->alloc_type == STATIC) {
       errno = EINVAL;
       return;
   }
   if (vec->data) free(vec->data);
   free(vec);
}
// --------------------------------------------------------------------------------

void _free_float_vector(float_v** vec) {
    if (vec && *vec) {
        free_float_vector(*vec);
        *vec = NULL;
    }
}
// -------------------------------------------------------------------------------- 

bool push_back_float_vector(float_v* vec, const float value) {
    if (vec == NULL|| vec->data == NULL) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
       
        // Allocate more space for the array of str structs
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        // Initialize new elements
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    vec->data[vec->len] = value; 
    vec->len++;
   
    return true;
}
// --------------------------------------------------------------------------------

bool push_front_float_vector(float_v* vec, const float value) {
    if (vec == NULL || vec->data == NULL) {
        errno = EINVAL;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        // Check for size_t overflow
        if (new_alloc > SIZE_MAX / sizeof(float)) {
            errno = ERANGE;
            return false;
        }
       
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }

    // Check for length overflow
    if (vec->len > SIZE_MAX - 1) {
        errno = ERANGE;
        return false;
    }
    
    // Move existing elements right if there are any
    if (vec->len > 0) {
        memmove(vec->data + 1, vec->data, vec->len * sizeof(float));
    }
    
    vec->data[0] = value;    
    vec->len++;
    return true;
}
// --------------------------------------------------------------------------------

bool insert_float_vector(float_v* vec, float value, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return false;
    }
    if (index > vec->len) {
        errno = ERANGE;
        return false;
    }
    if (vec->len >= SIZE_MAX) {  // Check length overflow
        errno = ERANGE;
        return false;
    }
   
    // Check if we need to resize
    if (vec->len >= vec->alloc) {
        if (vec->alloc_type == STATIC) {
            errno = EINVAL;
            return false;
        }
        size_t new_alloc = vec->alloc == 0 ? 1 : vec->alloc;
        if (new_alloc < VEC_THRESHOLD) {
            new_alloc *= 2;
        } else {
            new_alloc += VEC_FIXED_AMOUNT;
        }
        
        // Check allocation size overflow
        if (new_alloc > SIZE_MAX / sizeof(float)) {
            errno = ERANGE;
            return false;
        }
       
        float* new_data = realloc(vec->data, new_alloc * sizeof(float));
        if (!new_data) {
            errno = ENOMEM;
            return false;
        }
       
        memset(new_data + vec->alloc, 0, (new_alloc - vec->alloc) * sizeof(float));
       
        vec->data = new_data;
        vec->alloc = new_alloc;
    }
    
    // Move existing elements right
    if (index < vec->len) {  // Only move if not appending
        // Check for size_t overflow in move operation
        if (vec->len - index > SIZE_MAX - 1) {
            errno = ERANGE;
            return false;
        }
        memmove(vec->data + index + 1, vec->data + index, 
                (vec->len - index) * sizeof(float));
    }
    
    vec->data[index] = value;
    vec->len++;
    return true;
}
// -------------------------------------------------------------------------------- 


float pop_back_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
    
    // Create copy of last element
    float temp = vec->data[vec->len - 1];
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

float pop_front_float_vector(float_v* vec) {  // Fixed function name
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
   
    // Check for overflow in memmove size calculation
    if (vec->len > SIZE_MAX / sizeof(float)) {
        errno = ERANGE;
        return FLT_MAX;
    }
   
    // Create copy of first element
    float temp = vec->data[0];
    // Shift remaining elements left
    memmove(vec->data, vec->data + 1, (vec->len - 1) * sizeof(float));
   
    // Clear the last element (which was moved)
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
   
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

float pop_any_float_vector(float_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }
    
    if (index >= vec->len) {
        errno = ERANGE;
        return FLT_MAX;
    }
    
    // Create copy of element to pop
    float temp = vec->data[index];
    
    // If not the last element, shift remaining elements left
    if (index < vec->len - 1) {
        // Check for overflow in memmove size calculation
        if ((vec->len - index - 1) > SIZE_MAX / sizeof(float)) {
            errno = ERANGE;
            return FLT_MAX;
        }
        
        memmove(&vec->data[index], &vec->data[index + 1], 
                (vec->len - index - 1) * sizeof(float));
    }
   
    // Clear the last element
    memset(&vec->data[vec->len - 1], 0, sizeof(float));
    
    vec->len--;
    return temp;
}
// --------------------------------------------------------------------------------

const float float_vector_index(const float_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return FLT_MAX;
    }
    return vec->data[index];
}
// --------------------------------------------------------------------------------

const size_t float_vector_size(const float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->len;
}
// --------------------------------------------------------------------------------

const size_t float_vector_alloc(const float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    return vec->alloc;
}
// --------------------------------------------------------------------------------

void reverse_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return;
    }

    size_t i = 0;
    size_t j = vec->len - 1;
    while (i < j) {
       swap_float(&vec->data[i], &vec->data[j]);
       i++;
       j--;
    }
}
// ================================================================================
// ================================================================================ 
// QUICKSORT

void swap_float(float* a, float* b) {
    if (!a || !b) {
        errno = EINVAL;
        return;
    }
    float temp = *a;
    *a = *b;
    *b = temp;
}
// -------------------------------------------------------------------------------- 

static float* _median_of_three(float* a, float* b, float* c, iter_dir direction) {
    if ((direction == FORWARD && *a < *b) ||
        (direction == REVERSE && *a > *b)) {
        if ((direction == FORWARD && *b < *c) ||
            (direction == REVERSE && *b > *c))
            return b;
        if ((direction == FORWARD && *a < *c) ||
            (direction == REVERSE && *a > *c))
            return c;
        return a;
    }
    if ((direction == FORWARD && *a < *c) ||
        (direction == REVERSE && *a > *c))
        return a;
    if ((direction == FORWARD && *b < *c) ||
        (direction == REVERSE && *b > *c))
        return c;
    return b;
}
// -------------------------------------------------------------------------------- 

static void _insertion_sort(float* vec, int low, int high, iter_dir direction) {
    for (int i = low + 1; i <= high; i++) {
        float key = vec[i];
        int j = i - 1;
        while (j >= low && ((direction == FORWARD && vec[j] > key) ||
                           (direction == REVERSE && vec[j] < key))) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}
// --------------------------------------------------------------------------------

static int _partition_float(float* vec, int low, int high, iter_dir direction) {
    int mid = low + (high - low) / 2;
    float* pivot_ptr = _median_of_three(&vec[low], &vec[mid], &vec[high], direction);
    
    if (pivot_ptr != &vec[high])
        swap_float(pivot_ptr, &vec[high]);
    
    float pivot = vec[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if ((direction == FORWARD && vec[j] < pivot) ||
            (direction == REVERSE && vec[j] > pivot)) {
            i++;
            swap_float(&vec[i], &vec[j]);
        }
    }
    swap_float(&vec[i + 1], &vec[high]);
    return (i + 1);
}
// -------------------------------------------------------------------------------- 

static void _quicksort_float(float* vec, int low, int high, iter_dir direction) {
    while (low < high) {
        if (high - low < 10) {
            _insertion_sort(vec, low, high, direction);
            break;
        }
        
        int pi = _partition_float(vec, low, high, direction);
        
        if (pi - low < high - pi) {
            _quicksort_float(vec, low, pi - 1, direction);
            low = pi + 1;
        } else {
            _quicksort_float(vec, pi + 1, high, direction);
            high = pi - 1;
        }
    }
}
// -------------------------------------------------------------------------------- 

void sort_float_vector(float_v* vec, iter_dir direction) {
    if (!vec) {
        errno = EINVAL;
        return;
    }
    if (vec->len < 2) return;
    
    _quicksort_float(vec->data, 0, vec->len - 1, direction);
}
// -------------------------------------------------------------------------------- 

void trim_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return;
    }
    
    if (vec->alloc_type == STATIC || vec->len == vec->alloc) {
        return;
    }
   
    if (vec->len == 0) {
        errno = ENODATA;
        return;
    }

    // Check for overflow
    if (vec->len > SIZE_MAX / sizeof(float)) {
        errno = ERANGE;
        return;
    }
    
    float* ptr = realloc(vec->data, sizeof(float) * vec->len);
    if (ptr == NULL) {
        errno = ENOMEM;
        return;
    }
    
    vec->data = ptr;
    vec->alloc = vec->len;
}
// --------------------------------------------------------------------------------

size_t binary_search_float_vector(float_v* vec, float value, float tolerance, bool sort_first) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return LONG_MAX;
    }
    
    if (vec->len == 0) {
        errno = ENODATA;
        return LONG_MAX;
    }

    if (tolerance < 0) {
        errno = EINVAL;
        return LONG_MAX;
    }

    if (isnan(value) || isnan(tolerance)) {
        errno = EINVAL;
        return LONG_MAX;
    }
    
    // Sort if requested and vector has more than 1 element
    if (sort_first && vec->len > 1) {
        sort_float_vector(vec, FORWARD);
    }
    
    size_t left = 0;
    size_t right = vec->len - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        float diff = vec->data[mid] - value;
        
        // Check if we found a match within tolerance
        if (fabs(diff) <= tolerance) {
            return mid;
        }
        
        if (diff < 0) {
            left = mid + 1;
        } else {
            // Handle case where mid is 0 to prevent underflow
            if (mid == 0) {
                break;
            }
            right = mid - 1;
        }
    }
    // The value does not exist in the array 
    return LONG_MAX;
}
// -------------------------------------------------------------------------------- 

void update_float_vector(float_v* vec, size_t index, float replacement_value) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return;
    }
    if (index > vec->len -1) {
        errno = ERANGE;
        return;
    }
    vec->data[index] = replacement_value;
}
// -------------------------------------------------------------------------------- 

float min_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (vec->len == 1) return vec->data[0];
    float min = vec->data[0];
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] < min) min = vec->data[i];
    }
    return min;
}
// -------------------------------------------------------------------------------- 

float max_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }
    if (vec->len == 1) return vec->data[0];
    float max = vec->data[0];  // Changed variable name
    for (size_t i = 1; i < vec->len; i++) {
        if (vec->data[i] > max) max = vec->data[i];  // Changed comparison and variable
    }
    return max;  // Return max instead of min
}
// ================================================================================
// ================================================================================
// eof
