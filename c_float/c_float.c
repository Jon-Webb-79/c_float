// ================================================================================
// ================================================================================
// - File:    c_float.c
// - Purpose: This file contains the implementation for all functions used in 
//            the c_float library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 0.1
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <immintrin.h>  // AVX/SSE
#include "c_float.h"
#include <errno.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

static const float LOAD_FACTOR_THRESHOLD = 0.7;
static const size_t VEC_THRESHOLD = 1 * 1024 * 1024;  // 1 MB
static const size_t VEC_FIXED_AMOUNT = 1 * 1024 * 1024;  // 1 MB
static const size_t hashSize = 16;  //  Size fo hash map init functions
static const uint32_t HASH_SEED = 0x45d9f3b; 
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

float* c_float_ptr(float_v* vec) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return NULL;
    }
    return vec->data;
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

    float min_val = FLT_MAX;

#if defined(__AVX__)
    __m256 vmin = _mm256_set1_ps(min_val);
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        vmin = _mm256_min_ps(vmin, v);
    }

    __m128 low = _mm256_castps256_ps128(vmin);
    __m128 high = _mm256_extractf128_ps(vmin, 1);
    __m128 min128 = _mm_min_ps(low, high);
    min128 = _mm_min_ps(min128, _mm_movehl_ps(min128, min128));
    min128 = _mm_min_ps(min128, _mm_shuffle_ps(min128, min128, 0x1));
    min_val = _mm_cvtss_f32(min128);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#elif defined(__SSE__)
    __m128 vmin = _mm_set1_ps(min_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        vmin = _mm_min_ps(vmin, v);
    }

    vmin = _mm_min_ps(vmin, _mm_movehl_ps(vmin, vmin));
    vmin = _mm_min_ps(vmin, _mm_shuffle_ps(vmin, vmin, 0x1));
    min_val = _mm_cvtss_f32(vmin);

    for (; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];

#else
    for (size_t i = 0; i < vec->len; ++i)
        if (vec->data[i] < min_val)
            min_val = vec->data[i];
#endif

    return min_val;
}

// -------------------------------------------------------------------------------- 

float max_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    float max_val = -FLT_MAX;

#if defined(__AVX__)
    __m256 vmax = _mm256_set1_ps(max_val);
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        vmax = _mm256_max_ps(vmax, v);
    }

    __m128 low = _mm256_castps256_ps128(vmax);
    __m128 high = _mm256_extractf128_ps(vmax, 1);
    __m128 max128 = _mm_max_ps(low, high);
    max128 = _mm_max_ps(max128, _mm_movehl_ps(max128, max128));
    max128 = _mm_max_ps(max128, _mm_shuffle_ps(max128, max128, 0x1));
    max_val = _mm_cvtss_f32(max128);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#elif defined(__SSE__)
    __m128 vmax = _mm_set1_ps(max_val);
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        vmax = _mm_max_ps(vmax, v);
    }

    vmax = _mm_max_ps(vmax, _mm_movehl_ps(vmax, vmax));
    vmax = _mm_max_ps(vmax, _mm_shuffle_ps(vmax, vmax, 0x1));
    max_val = _mm_cvtss_f32(vmax);

    for (; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];

#else
    for (size_t i = 0; i < vec->len; ++i)
        if (vec->data[i] > max_val)
            max_val = vec->data[i];
#endif

    return max_val;
}

// -------------------------------------------------------------------------------- 

float sum_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    const size_t len = vec->len;
    const float* data = vec->data;

    float sum = 0.0f;

#if defined(__AVX__)
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 7 < len; i += 8) {
        __m256 chunk = _mm256_loadu_ps(&data[i]);
        vsum = _mm256_add_ps(vsum, chunk);
    }

    // Horizontal sum of vsum
    __m128 low  = _mm256_castps256_ps128(vsum);         // lower 128
    __m128 high = _mm256_extractf128_ps(vsum, 1);       // upper 128
    __m128 sum128 = _mm_add_ps(low, high);              // add lower + upper

    // Shuffle and reduce
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum += _mm_cvtss_f32(sum128);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += data[i];
    }

#elif defined(__SSE__)
    __m128 vsum = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 3 < len; i += 4) {
        __m128 chunk = _mm_loadu_ps(&data[i]);
        vsum = _mm_add_ps(vsum, chunk);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);
    sum += _mm_cvtss_f32(vsum);

    for (; i < len; ++i) {
        sum += data[i];
    }

#else
    // Fallback to scalar
    for (size_t i = 0; i < len; ++i) {
        sum += data[i];
    }
#endif

    return sum;
}

// -------------------------------------------------------------------------------- 

float average_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return FLT_MAX;
    }

    float sum = sum_float_vector(vec);
    if (errno != 0) return FLT_MAX;
    return sum / vec->len;
}

// -------------------------------------------------------------------------------- 

float stdev_float_vector(float_v* vec) {
    if (!vec || !vec->data) {
        errno = ENODATA;
        return FLT_MAX;
    }

    if (vec->len == 0) {
        errno = ENODATA;
        return FLT_MAX;
    }

    if (vec->len == 1) {
        errno = ENODATA;
        return FLT_MAX;
    }

    float mean = average_float_vector(vec);
    if (errno != 0) return FLT_MAX;

    float sum_sq_diff = 0.0f;

#if defined(__AVX__)
    __m256 vmean = _mm256_set1_ps(mean);
    __m256 vsum = _mm256_setzero_ps();
    size_t i = 0;

    for (; i + 7 < vec->len; i += 8) {
        __m256 v = _mm256_loadu_ps(&vec->data[i]);
        __m256 diff = _mm256_sub_ps(v, vmean);
        __m256 sq = _mm256_mul_ps(diff, diff);
        if (!_mm256_testz_ps(sq, sq)) { // Skip all-zero check — we need isinf
            for (int j = 0; j < 8; ++j) {
                float x = vec->data[i + j];
                if (isinf(x)) return INFINITY;
            }
        }
        vsum = _mm256_add_ps(vsum, sq);
    }

    __m128 low = _mm256_castps256_ps128(vsum);
    __m128 high = _mm256_extractf128_ps(vsum, 1);
    __m128 sum128 = _mm_add_ps(low, high);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum128 = _mm_hadd_ps(sum128, sum128);
    sum_sq_diff += _mm_cvtss_f32(sum128);

    for (; i < vec->len; ++i) {
        float diff = vec->data[i] - mean;
        float sq = diff * diff;
        if (isinf(vec->data[i])) return INFINITY;
        sum_sq_diff += sq;
    }

#elif defined(__SSE__)
    __m128 vmean = _mm_set1_ps(mean);
    __m128 vsum = _mm_setzero_ps();
    size_t i = 0;

    for (; i + 3 < vec->len; i += 4) {
        __m128 v = _mm_loadu_ps(&vec->data[i]);
        __m128 diff = _mm_sub_ps(v, vmean);
        __m128 sq = _mm_mul_ps(diff, diff);
        for (int j = 0; j < 4; ++j) {
            if (isinf(vec->data[i + j])) return INFINITY;
        }
        vsum = _mm_add_ps(vsum, sq);
    }

    vsum = _mm_hadd_ps(vsum, vsum);
    vsum = _mm_hadd_ps(vsum, vsum);
    sum_sq_diff += _mm_cvtss_f32(vsum);

    for (; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        float diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }

#else
    for (size_t i = 0; i < vec->len; ++i) {
        if (isinf(vec->data[i])) return INFINITY;
        float diff = vec->data[i] - mean;
        sum_sq_diff += diff * diff;
    }
#endif

    return sqrtf(sum_sq_diff / vec->len);
}

// -------------------------------------------------------------------------------- 

float_v* cum_sum_float_vector(float_v* vec) {
    if (!vec || !vec->data || vec->len == 0) {
        errno = EINVAL;
        return NULL;
    }

    float_v* new_vec = init_float_vector(vec->len);
    if (!new_vec) {
        errno = ENOMEM;
        return NULL;
    }

    float sum = 0.0f;
    for (size_t i = 0; i < vec->len; ++i) {
        float val = vec->data[i];
        if (isnan(val)) {
            errno = EINVAL;
            free_float_vector(new_vec);
            return NULL;
        }

        sum += val;

        if (isinf(sum)) {
            // Fill rest with infinity
            for (; i < vec->len; ++i) {
                push_back_float_vector(new_vec, INFINITY);
            }
            return new_vec;
        }

        if (!push_back_float_vector(new_vec, sum)) {
            free_float_vector(new_vec);
            return NULL;
        }
    }

    return new_vec;
}
// -------------------------------------------------------------------------------- 

float_v* copy_float_vector(const float_v* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    float_v* copy = init_float_vector(original->alloc);
    if (!copy) {
        return NULL;
    }

    for (size_t i = 0; i < original->len; ++i) {
        if (!push_back_float_vector(copy, original->data[i])) {
            free_float_vector(copy);
            return NULL;
        }
    }

    return copy;
}
// ================================================================================
// ================================================================================
// DICTIONARY IMPLEMENTATION

typedef struct fdictNode {
    char* key;
    float value;
    struct fdictNode* next;
} fdictNode;
// --------------------------------------------------------------------------------

struct dict_f {
    fdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

/**
 * @brief MurmurHash3-inspired hash function for strings
 * 
 * @param key The string key to hash
 * @param seed Optional seed for hash randomization (helps prevent hash flooding)
 * @return size_t The computed hash value
 */
static size_t hash_function(const char* key, const uint32_t seed) {
    if (!key) {
        return 0;
    }

    // Constants for mixing
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    uint32_t h1 = seed;

    // Process key in 4-byte chunks
    const unsigned char* data = (const unsigned char*)key;
    size_t len = strlen(key);
    const size_t nblocks = len / 4;

    // Body
    const uint32_t* blocks = (const uint32_t*)(data);
    for (size_t i = 0; i < nblocks; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1, 15)
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);  // ROTL32(h1, 13)
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail
    const unsigned char* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3:
            k1 ^= tail[2] << 16;
            /* fallthrough */
        case 2:
            k1 ^= tail[1] << 8;
            /* fallthrough */
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17);  // ROTL32(k1, 15)
            k1 *= c2;
            h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return (size_t)h1;
}
// --------------------------------------------------------------------------------

/**
 * @brief Resizes the dictionary's hash table
 * 
 * @param dict Pointer to the dictionary
 * @param new_size Desired new size for the hash table
 * @return bool true if resize successful, false otherwise
 */
static bool resize_dict(dict_f* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    fdictNode* new_table = calloc(new_size, sizeof(fdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table in case we need to recover
    fdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash existing entries
    for (size_t i = 0; i < old_size; i++) {
        fdictNode* current = old_table[i].next;
        
        while (current) {
            fdictNode* next = current->next;  // Save next pointer before modifying node

            // Calculate new index using enhanced hash function
            size_t new_index = hash_function(current->key, HASH_SEED) % new_size;

            // Insert at the beginning of the new chain
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;
            
            rehashed_count++;
            current = next;
        }
    }

    // Verify all entries were rehashed successfully
    if (rehashed_count != dict->hash_size) {
        // Cleanup and restore old state
        for (size_t i = 0; i < new_size; i++) {
            fdictNode* current = new_table[i].next;
            while (current) {
                fdictNode* next = current->next;
                current->next = NULL;
                current = next;
            }
        }
        free(new_table);
        errno = EAGAIN;
        return false;
    }

    // Update the dictionary only after successful rehashing
    dict->keyValues = new_table;
    dict->alloc = new_size;

    // Clean up old table (but not the nodes, as they were moved)
    free(old_table);

    return true;
}
// --------------------------------------------------------------------------------

dict_f* init_float_dict(void) {
    // Allocate the dictionary structure
    dict_f* dict = calloc(1, sizeof(dict_f));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(fdictNode));
    if (!dict->keyValues) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate hash table array\n");
        free(dict);
        return NULL;
    }

    // Initialize dictionary metadata
    dict->hash_size = 0;   // No items yet
    dict->len = 0;         // No occupied buckets
    dict->alloc = hashSize;

    return dict;
}
// --------------------------------------------------------------------------------

bool insert_float_dict(dict_f* dict, const char* key, float value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    // Check load factor and resize if necessary using adaptive growth strategy
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size;
        if (dict->alloc < VEC_THRESHOLD) {
            new_size = dict->alloc * 2;  // Double when small
        } else {
            new_size = dict->alloc + VEC_FIXED_AMOUNT;  // Linear growth when large
        }
        
        if (!resize_dict(dict, new_size)) {
            return false;  // resize_dict sets appropriate errno
        }
    }

    // Rest of the function remains the same as previous improvement...
    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    // Check for existing key
    for (fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    fdictNode* new_node = malloc(sizeof(fdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// --------------------------------------------------------------------------------

float pop_float_dict(dict_f* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    fdictNode* prev = &dict->keyValues[index];
    fdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Save value and unlink node
            float value = current->value;
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free(current->key);
            free(current);
            
            return value;
        }
        prev = current;
        current = current->next;
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

float get_float_dict_value(const dict_f* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return FLT_MAX;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return FLT_MAX;
}
// --------------------------------------------------------------------------------

void free_float_dict(dict_f* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        fdictNode* current = dict->keyValues[i].next;
        while (current) {      
            fdictNode* next = current->next;  // Save next pointer before freeing
            free(current->key);
            free(current);
            current = next;
        }
    }

    // Free the hash table and dictionary struct
    free(dict->keyValues);
    free(dict);
}
// -------------------------------------------------------------------------------- 

void _free_float_dict(dict_f** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_float_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// --------------------------------------------------------------------------------

bool update_float_dict(dict_f* dict, const char* key, float value) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return true;
        }
    }

    errno = ENOENT;  // More specific error code for missing key
    return false;
}
// --------------------------------------------------------------------------------

size_t float_dict_size(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t float_dict_alloc(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// --------------------------------------------------------------------------------

size_t float_dict_hash_size(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

bool has_key_float_dict(const dict_f* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const fdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

dict_f* copy_float_dict(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with same capacity
    dict_f* new_dict = malloc(sizeof(dict_f));
    if (!new_dict) {
        errno = ENOMEM;
        return NULL;
    }

    new_dict->keyValues = calloc(dict->alloc, sizeof(fdictNode));
    if (!new_dict->keyValues) {
        free(new_dict);
        errno = ENOMEM;
        return NULL;
    }

    // Copy metadata
    new_dict->alloc = dict->alloc;
    new_dict->hash_size = 0;  // Will be incremented during insertion
    new_dict->len = 0;        // Will be incremented during insertion

    // Copy all entries
    for (size_t i = 0; i < dict->alloc; i++) {
        fdictNode* current = dict->keyValues[i].next;
        while (current) {
            // Insert will handle incrementing hash_size and len
            if (!insert_float_dict(new_dict, current->key, current->value)) {
                free_float_dict(new_dict);  // Clean up on failure
                return NULL;
            }
            current = current->next;
        }
    }

    return new_dict;
}
// -------------------------------------------------------------------------------- 

bool clear_float_dict(dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return false;
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        fdictNode* current = dict->keyValues[i].next;
        while (current) {
            fdictNode* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
        dict->keyValues[i].next = NULL;  // Reset bucket head
    }

    // Reset dictionary metadata
    dict->hash_size = 0;
    dict->len = 0;

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_float_dict(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    string_v* vec = init_str_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    for (size_t i = 0; i < dict->alloc; i++) {
        for (fdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_str_vector(vec, current->key)) {
                free_str_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

float_v* get_values_float_dict(const dict_f* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }
    float_v* vec = init_float_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }
    // Iterate through all buckets
    for (size_t i = 0; i < dict->alloc; i++) {
        for (fdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_float_vector(vec, current->value)) {
                free_float_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }
    return vec;
}
// -------------------------------------------------------------------------------- 

dict_f* merge_float_dict(const dict_f* dict1, const dict_f* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Create new dictionary with capacity for all items
    //size_t initial_size = dict1->hash_size + dict2->hash_size;
    dict_f* merged = init_float_dict();
    if (!merged) {
        return NULL;  // errno set by init_float_dict
    }

    // First, copy all entries from dict1
    for (size_t i = 0; i < dict1->alloc; i++) {
        for (fdictNode* current = dict1->keyValues[i].next; current; current = current->next) {
            if (!insert_float_dict(merged, current->key, current->value)) {
                free_float_dict(merged);
                return NULL;
            }
        }
    }

    // Then handle dict2 entries
    for (size_t i = 0; i < dict2->alloc; i++) {
        for (fdictNode* current = dict2->keyValues[i].next; current; current = current->next) {
            float existing_value;
            // Check if key exists in merged dictionary
            if ((existing_value = get_float_dict_value(merged, current->key)) != FLT_MAX) {
                if (overwrite) {
                    // Update existing value if overwrite is true
                    if (!update_float_dict(merged, current->key, current->value)) {
                        free_float_dict(merged);
                        return NULL;
                    }
                }
                // If overwrite is false, keep original value
            } else {
                // Key doesn't exist, insert new entry
                if (!insert_float_dict(merged, current->key, current->value)) {
                    free_float_dict(merged);
                    return NULL;
                }
            }
        }
    }

    return merged;
}
// --------------------------------------------------------------------------------

bool foreach_float_dict(const dict_f* dict, dict_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; i++) {
        for (fdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            iter(current->key, current->value, user_data);
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================

typedef struct fvdictNode {
    char* key;
    float_v* value;
    struct fvdictNode* next;
} fvdictNode;
// --------------------------------------------------------------------------------

struct dict_fv {
    fvdictNode* keyValues;
    size_t hash_size;
    size_t len;
    size_t alloc;
};
// --------------------------------------------------------------------------------

dict_fv* init_floatv_dict(void) {
    // Allocate the dictionary structure
    dict_fv* dict = calloc(1, sizeof(dict_f));
    if (!dict) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate vector dictionary structure\n");
        return NULL;
    }

    // Allocate initial hash table array
    dict->keyValues = calloc(hashSize, sizeof(fvdictNode));
    if (!dict->keyValues) {
        errno = ENOMEM;
        fprintf(stderr, "Failed to allocate hash table array\n");
        free(dict);
        return NULL;
    }

    // Initialize dictionary metadata
    dict->hash_size = 0;   // No items yet
    dict->len = 0;         // No occupied buckets
    dict->alloc = hashSize;

    return dict;
}
// -------------------------------------------------------------------------------- 

static bool resize_dictv(dict_fv* dict, size_t new_size) {
    // Input validation
    if (!dict || new_size < dict->hash_size || new_size == 0) {
        errno = EINVAL;
        return false;
    }

    // Ensure new_size is a power of 2 for better hash distribution
    new_size = (size_t)pow(2, ceil(log2(new_size)));

    // Use calloc for automatic zero initialization
    fvdictNode* new_table = calloc(new_size, sizeof(fvdictNode));
    if (!new_table) {
        errno = ENOMEM;
        return false;
    }

    // Keep track of old table for cleanup if something fails
    fvdictNode* old_table = dict->keyValues;
    const size_t old_size = dict->alloc;
    size_t rehashed_count = 0;

    // Rehash all existing entries into the new table
    for (size_t i = 0; i < old_size; ++i) {
        fvdictNode* current = old_table[i].next;
        while (current) {
            fvdictNode* next = current->next;

            size_t new_index = hash_function(current->key, HASH_SEED) % new_size;

            // Reinsert into the new hash bucket (head insertion)
            current->next = new_table[new_index].next;
            new_table[new_index].next = current;

            rehashed_count++;
            current = next;
        }
    }

    // Validate rehash count matches the number of entries
    if (rehashed_count != dict->hash_size) {
        // Rollback: disconnect moved nodes so they aren't double-freed
        for (size_t i = 0; i < new_size; ++i) {
            fvdictNode* current = new_table[i].next;
            while (current) {
                fvdictNode* next = current->next;
                current->next = NULL;
                current = next;
            }
        }
        free(new_table);
        errno = EAGAIN;
        return false;
    }

    // Replace table on success
    dict->keyValues = new_table;
    dict->alloc = new_size;

    // Free old hash bucket array (nodes were moved, not freed)
    free(old_table);

    return true;
}
// --------------------------------------------------------------------------------

bool create_floatv_dict(dict_fv* dict, char* key, size_t size) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    // Resize if load factor exceeded
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size = (dict->alloc < VEC_THRESHOLD)
                          ? dict->alloc * 2
                          : dict->alloc + VEC_FIXED_AMOUNT;

        if (!resize_dictv(dict, new_size)) {
            return false;
        }
    }

    const size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    // Check for key collision
    for (fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    fvdictNode* new_node = malloc(sizeof(fvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    float_v* value = NULL;
    value = init_float_vector(size);
    if (!value) {
        free(new_key);
        free(new_node);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// --------------------------------------------------------------------------------

bool pop_floatv_dict(dict_fv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    fvdictNode* prev = &dict->keyValues[index];
    fvdictNode* current = prev->next;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            prev->next = current->next;
            
            // Update dictionary metadata
            dict->hash_size--;
            if (!prev->next) {  // If bucket is now empty
                dict->len--;
            }
            
            // Clean up node memory
            free_float_vector(current->value);
            free(current->key);
            free(current);
            
            return true;
        }
        prev = current;
        current = current->next;
    }

    errno = ENOENT;  // Set errno when key not found
    return false;
}
// -------------------------------------------------------------------------------- 

float_v* return_floatv_pointer(dict_fv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return NULL;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;
    
    for (const fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
    }

    errno = ENOENT;  // Set errno when key not found
    return NULL;
}
// -------------------------------------------------------------------------------- 

void free_floatv_dict(dict_fv* dict) {
    if (!dict) {
        return;  // Silent return on NULL - common pattern for free functions
    }

    // Free all nodes in each bucket
    for (size_t i = 0; i < dict->alloc; i++) {
        fvdictNode* current = dict->keyValues[i].next;
        while (current) {      
            fvdictNode* next = current->next;

            free_float_vector(current->value);
            free(current->key);
            free(current);

            current = next;
        }
    }

    // Free the hash table and dictionary struct
    free(dict->keyValues);
    free(dict);
}
// -------------------------------------------------------------------------------- 

void _free_floatv_dict(dict_fv** dict_ptr) {
    if (dict_ptr && *dict_ptr) {
        free_floatv_dict(*dict_ptr);
        *dict_ptr = NULL;  // Prevent use-after-free
    }
}
// -------------------------------------------------------------------------------- 

bool has_key_floatv_dict(const dict_fv* dict, const char* key) {
    if (!dict || !key) {
        errno = EINVAL;
        return false;
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    for (const fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool insert_floatv_dict(dict_fv* dict, const char* key, float_v* value) {
    if (!dict || !key || !value) {
        errno = EINVAL;
        return false;
    }

    // Only allow insertion of DYNAMIC vectors
    if (value->alloc_type != DYNAMIC) {
        errno = EPERM;  // Operation not permitted
        return false;
    }

    // Resize if load factor exceeded
    if (dict->hash_size >= dict->alloc * LOAD_FACTOR_THRESHOLD) {
        size_t new_size = (dict->alloc < VEC_THRESHOLD)
                          ? dict->alloc * 2
                          : dict->alloc + VEC_FIXED_AMOUNT;

        if (!resize_dictv(dict, new_size)) {
            return false;
        }
    }

    size_t index = hash_function(key, HASH_SEED) % dict->alloc;

    // Check for existing key
    for (fvdictNode* current = dict->keyValues[index].next; current; current = current->next) {
        if (strcmp(current->key, key) == 0) {
            errno = EEXIST;
            return false;
        }
    }

    // Allocate new key string
    char* new_key = strdup(key);
    if (!new_key) {
        errno = ENOMEM;
        return false;
    }

    // Allocate node
    fvdictNode* new_node = malloc(sizeof(fvdictNode));
    if (!new_node) {
        free(new_key);
        errno = ENOMEM;
        return false;
    }

    new_node->key = new_key;
    new_node->value = value;
    new_node->next = dict->keyValues[index].next;
    dict->keyValues[index].next = new_node;

    dict->hash_size++;
    if (new_node->next == NULL) {
        dict->len++;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

size_t float_dictv_size(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->len;
}
// --------------------------------------------------------------------------------

size_t float_dictv_alloc(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->alloc;
}
// -------------------------------------------------------------------------------- 

size_t float_dictv_hash_size(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return SIZE_MAX;
    }
    return dict->hash_size;
}
// -------------------------------------------------------------------------------- 

dict_fv* copy_floatv_dict(const dict_fv* original) {
    if (!original) {
        errno = EINVAL;
        return NULL;
    }

    dict_fv* copy = init_floatv_dict();
    if (!copy) {
        return NULL;  // errno already set
    }

    for (size_t i = 0; i < original->alloc; ++i) {
        fvdictNode* current = original->keyValues[i].next;
        while (current) {
            // if (current->value->alloc_type != DYNAMIC) {
            //     free_floatv_dict(copy);
            //     errno = EPERM;
            //     return NULL;
            // }

            float_v* vec_copy = copy_float_vector(current->value);
            if (!vec_copy) {
                free_floatv_dict(copy);
                return NULL;
            }

            if (!insert_floatv_dict(copy, current->key, vec_copy)) {
                free_float_vector(vec_copy);
                free_floatv_dict(copy);
                return NULL;
            }

            current = current->next;
        }
    }

    return copy;
}
// -------------------------------------------------------------------------------- 

dict_fv* merge_floatv_dict(const dict_fv* dict1, const dict_fv* dict2, bool overwrite) {
    if (!dict1 || !dict2) {
        errno = EINVAL;
        return NULL;
    }

    // Start by deep copying dict1
    dict_fv* merged = copy_floatv_dict(dict1);
    if (!merged) {
        return NULL;
    }

    // Now process dict2 entries
    for (size_t i = 0; i < dict2->alloc; ++i) {
        fvdictNode* current = dict2->keyValues[i].next;
        while (current) {
            if (!current->key || !current->value || current->value->alloc_type != DYNAMIC) {
                free_floatv_dict(merged);
                errno = EPERM;
                return NULL;
            }

            bool exists = has_key_floatv_dict(merged, current->key);
            if (exists && !overwrite) {
                current = current->next;
                continue;
            }

            float_v* vec_copy = copy_float_vector(current->value);
            if (!vec_copy) {
                free_floatv_dict(merged);
                return NULL; // errno set by copy_float_vector
            }

            if (exists) {
                pop_floatv_dict(merged, current->key);
            }

            if (!insert_floatv_dict(merged, current->key, vec_copy)) {
                free_float_vector(vec_copy);
                free_floatv_dict(merged);
                return NULL;
            }

            current = current->next;
        }
    }

    return merged;
}
// -------------------------------------------------------------------------------- 

void clear_floatv_dict(dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        fvdictNode* current = dict->keyValues[i].next;
        dict->keyValues[i].next = NULL;

        while (current) {
            fvdictNode* next = current->next;

            if (current->value) {
                if (current->value->alloc_type == STATIC) {
                    free(current->value);
                } else {
                    free_float_vector(current->value);
                }
            }

            free(current->key);
            free(current);
            current = next;
        }
    }

    dict->hash_size = 0;
    dict->len = 0;
}
// -------------------------------------------------------------------------------- 

bool foreach_floatv_dict(const dict_fv* dict, dict_fv_iterator iter, void* user_data) {
    if (!dict || !iter) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        fvdictNode* current = dict->keyValues[i].next;
        while (current) {
            iter(current->key, current->value, user_data);
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

string_v* get_keys_floatv_dict(const dict_fv* dict) {
    if (!dict) {
        errno = EINVAL;
        return NULL;
    }

    string_v* vec = init_str_vector(dict->hash_size);
    if (!vec) {
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < dict->alloc; ++i) {
        for (fvdictNode* current = dict->keyValues[i].next; current; current = current->next) {
            if (!push_back_str_vector(vec, current->key)) {
                free_str_vector(vec);
                errno = ENOMEM;
                return NULL;
            }
        }
    }

    return vec;
}
// ================================================================================ 
// ================================================================================ 
// eof
