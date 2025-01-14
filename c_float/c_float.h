// ================================================================================
// ================================================================================
// - File:    c_float.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_float_H
#define c_float_H
// ================================================================================
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================
#ifndef ITER_DIR_H
#define ITER_DIR_H
    /**
     * @brief An enum containing keywords for an iterator 
     *
     * This enum contains keywords that are used to describe the order of iteration
     * within an iterator 
     *
     * @attribute FORWARD Keyword to command a forward iteration 
     * @attribute REVERSE Keyword to command a reverese iteration
     */
    typedef enum {
        FORWARD,
        REVERSE
    }iter_dir;
#endif /* ITER_DIR_H*/
// --------------------------------------------------------------------------------    

#ifndef ALLOC_H 
#define ALLOC_H 

    /**
     * @enum alloc_t 
     * @brief An enum to discern if an array is statically or allocated 
     */
    typedef enum {
        STATIC,
        DYNAMIC
    } alloc_t;

#endif /*ALLOC_H*/

/**
* @struct float_v
* @brief Dynamic array (vector) container for float objects
*
* This structure manages a resizable array of float objects with automatic
* memory management and capacity handling.
*/
typedef struct {
    float* data;
    size_t len;
    size_t alloc;
    alloc_t alloc_type;
} float_v;
// --------------------------------------------------------------------------------

/**
* @function init_float_vector
* @brief Initializes a new dynamically allocated float vector with specified initial capacity
*
* @param buffer Initial capacity to allocate
* @return Pointer to new float_v object, or NULL on allocation failure
*         Sets errno to ENOMEM if memory allocation fails
*/
float_v* init_float_vector(size_t buffer);
// --------------------------------------------------------------------------------

/**
* @macro init_float_array
* @brief Initializes a new statically allocated float vector with specified initial capacity
*
* @param buffer Total capacity to allocate
*/
#define init_float_array(buffer) {.data = float[buffer]; .len = 0; .alloc = buffer; .alloc_type = STATIC}
// --------------------------------------------------------------------------------

/**
* @function push_back_float_vector
* @brief Adds a float value to the end of the vector
*
* Automatically resizes the vector if necessary.
*
* @param vec Target float vector
* @param value Float value to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_back_float_vector(float_v* vec, const float value);
// --------------------------------------------------------------------------------

/**
* @function push_front_float_vector
* @brief Adds a float value to the beginning of the vector
*
* Shifts existing elements right and automatically resizes if necessary.
*
* @param vec Target float vector
* @param value Float value to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_front_float_vector(float_v* vec, const float value);
// --------------------------------------------------------------------------------

/**
* @function insert_float_vector
* @brief Inserts a float value at specified index in the vector
*
* Shifts elements right starting at index and resizes if necessary.
*
* @param vec Target float vector
* @param value Float value to insert
* @param index Position to insert at
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or index out of bounds, ENOMEM on allocation failure
*/
bool insert_float_vector(float_v* vec, const float value, size_t index);
// --------------------------------------------------------------------------------

/**
* @function float_vector_index
* @brief Retrieves pointer to string_t at specified index
*
* @param vec Source float vector
* @param index Position to access
* @return Pointer to string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
const float float_vector_index(const float_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
* @function float_vector_size
* @brief Returns current number of strings in vector
*
* @param vec Float vector to query
* @return Number of strings in vector, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t float_vector_size(const float_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function float_vector_alloc
* @brief Returns current allocation size of vector
*
* @param vec Float vector to query
* @return Current allocation capacity, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t float_vector_alloc(const float_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pop_back_float_vector
* @brief Removes and returns last float value in vector
*
* @param vec Source float vector
* @return Pointer to removed float object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
float pop_back_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function pop_front_float_vector
* @brief Removes and returns first float value in vector
*
* Shifts remaining elements left.
*
* @param vec Source string vector
* @return Pointer to removed float object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
float pop_front_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pup_any_float_vector
* @brief Removes and returns float value at specified index
*
* Shifts remaining elements left to fill gap.
*
* @param vec Source float vector
* @param index Position to remove from
* @return Pointer to removed float_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
float pop_any_float_vector(float_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function free_float_vector
* @brief Frees all memory associated with string vector
*
* Frees all contained strings and the vector itself.
*
* @param vec Float vector to free
* @return void
*         Sets errno to EINVAL for NULL input
*/
void free_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
* @function _free_float_vector
* @brief Helper function for garbage collection of float vectors
*
* Used with FLTVEC_GBC macro for automatic cleanup.
*
* @param vec Double pointer to float vector to free
* @return void
*/
void _free_float_vector(float_v** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro FLTVEC_GBC
     * @brief A macro for enabling automatic cleanup of float vector objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_float_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define FLTVEC_GBC __attribute__((cleanup(_free_float_vector)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @function reverse_float_vector
 * @brief Reverses the order of elements in a float vector in place.
 *
 * The function reverses the order of elements by swapping elements from the
 * beginning and end of the vector until the middle is reached.
 *
 * @param vec float vector to reverse
 * @return void
 *         Sets errno to EINVAL if vec is NULL or invalid
 */
void reverse_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @function swap_float
 * @brief Swaps the contents of two float objects.
 *
 * Performs an in-place swap of float values
 *
 * @param a Pointer to first float object
 * @param b Pointer to second float object
 * @return void
 *         Sets errno to EINVAL if either input is NULL
 */
void swap_float(float* a, float* b);
// --------------------------------------------------------------------------------

/**
* @function sort_float_vector
* @brief Sorts a float vector in ascending or descending order.
*
* Uses an optimized QuickSort algorithm with median-of-three pivot selection
* and insertion sort for small subarrays. Sort direction is determined by
* the iter_dir parameter.
*
* @param vec float vector to sort
* @param direction FORWARD for ascending order, REVERSE for descending
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void sort_float_vector(float_v* vec, iter_dir direction);
// ================================================================================
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_float_H */
// ================================================================================
// ================================================================================
// eof
