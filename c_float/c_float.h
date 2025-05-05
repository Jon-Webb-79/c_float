// ================================================================================
// ================================================================================
// - File:    c_float.h
// - Purpose: This file contains prototypes for the c_float functions in the 
//            c_float.c file.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 0.1
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_float_H
#define c_float_H
// ================================================================================
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
#include "c_string.h"
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
 * @function init_float_static_array
 * @brief Creates a stack-based float vector with static array
 *
 * @param size Size of the array
 */
#define init_float_array(size) \
    ((float_v){.data = (float[size]){0}, .len = 0, .alloc = size, .alloc_type = STATIC})
// -------------------------------------------------------------------------------- 

/**
 * @brief returns a c style pointer to the beginning of an array 
 *
 * @param vec A pointer to a float_v data type 
 * @return A pointer to the beginning of a float array
 */
float* c_float_ptr(float_v* vec);
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
// -------------------------------------------------------------------------------- 

/**
* @function trim_float_vector
* @brief Trims all un-necessary memory from a vector
*
* @param vec float vector to trim
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void trim_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function binary_search_float_vector
* @brief Searches a float vector to find the index where a value exists
*
* @param vec float vector object
* @param value The value to search for
* @param tolerance The float tolerance for finding a value 
* @param sort_first true if the vector or array needs to be sorted, false otherwise
* @return The index where a value exists, LONG_MAX if the value is not in the array.
*         Sets errno to EINVAL if vec is NULL or invalid, ENODATA if the array is 
*         not populated
*/
size_t binary_search_float_vector(float_v* vec, float value, float tolerance, bool sort_first);
// -------------------------------------------------------------------------------- 

/**
* @function update_float_vector
* @brief Replaces the value of a vector at a specific index
*
* @param vec float vector object
* @param index The index where data will be replaced
* @param replacement_value The replacement value
* @return void, Sets errno to EINVAL if vec does not exsist, or ERANGE 
*         if the index is out of bounds
*/
void update_float_vector(float_v* vec, size_t index, float replacement_value);
// -------------------------------------------------------------------------------- 

/**
 * @function min_float_vector 
 * @brief Returns the minimum value in a vector or array 
 *
 * @param vec A float vector or array object 
 * @return The minimum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
float min_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function max_float_vector 
 * @brief Returns the maximum value in a vector or array 
 *
 * @param vec A float vector or array object 
 * @return The maximum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
float max_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function sum_float_vector 
 * @brief Returns the summation of all values in a vector or array
 *
 * @param vec A float vector or array object 
 * @return The summation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
float sum_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function average_float_vector 
 * @brief Returns the average of all values in a vector or array
 *
 * @param vec A float vector or array object 
 * @return The average of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
float average_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function stdev_float_vector 
 * @brief Returns the standard deviation of all values in a vector or array
 *
 * @param vec A float vector or array object 
 * @return The standard deviation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
float stdev_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function cum_sum_float_vector 
 * @brief Returns a dynamically allocated array containing the cumulative sum of all 
 *        values in vec
 *
 * @param vec A float vector or array object 
 * @return A float_v object with the cumulative sum of all values in vec.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
float_v* cum_sum_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief creates a deep copy of a vector
 *
 * @param original A vector to be copied 
 * @return A copy of a float vector
 */
float_v* copy_float_vector(const float_v* original);
// ================================================================================ 
// ================================================================================ 
// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_f
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to float values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_f dict_f;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_f* init_float_dict();
// --------------------------------------------------------------------------------

/**
 * @brief Inserts a key-value pair into the dictionary.
 *
 * Adds a new key-value pair to the dictionary. If the key already exists, the function
 * does nothing and returns false. If the dictionary's load factor exceeds a threshold,
 * it automatically resizes.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to insert.
 * @param value The value associated with the key.
 * @return true if the key-value pair was inserted successfully, false otherwise.
 */
bool insert_float_dict(dict_f* dict, const char* key, float value);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a key-value pair from the dictionary.
 *
 * Finds the specified key in the dictionary, removes the associated key-value pair,
 * and returns the value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to remove.
 * @return The value associated with the key if it was found and removed; FLT_MAX otherwise.
 */
float pop_float_dict(dict_f* dict,  const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Retrieves the value associated with a key.
 *
 * Searches the dictionary for the specified key and returns the corresponding value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to search for.
 * @return The value associated with the key, or FLT_MAX if the key is not found.
 */
float get_float_dict_value(const dict_f* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_float_dict(dict_f* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_float_dict(dict_f** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_t objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define FDICT_GBC __attribute__((cleanup(_free_float_dict)))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Updates the value associated with a key in the dictionary.
 *
 * Searches for the specified key in the dictionary and updates its value.
 * If the key does not exist, the function takes no action.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to update.
 * @param value The new value to associate with the key.
 */
bool update_float_dict(dict_f* dict, const char* key, float value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t float_dict_size(const dict_f* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t float_dict_alloc(const dict_f* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t float_dict_hash_size(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_float_dict(const dict_f* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_f* New dictionary containing copies of all entries, NULL on error
 */
dict_f* copy_float_dict(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_float_dict(dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_float_dict(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return float* Array containing all values, NULL on error
 */
float_v* get_values_float_dict(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_f* New dictionary containing merged entries, NULL on error
 */
dict_f* merge_float_dict(const dict_f* dict1, const dict_f* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*dict_iterator)(const char* key, float value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_float_dict(const dict_f* dict, dict_iterator iter, void* user_data);
// ================================================================================ 
// ================================================================================
// VECTOR DICTIONARY PROTOTYPES 

/**
 * @typedef dict_fv
 * @brief Opaque struct representing a dictionary for vector data types.
 *
 * This structure encapsulates a hash table that maps string keys to a vector of float values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_fv dict_fv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of float vectors
 *
 * @returns a dictionary of float vectors
 */
dict_fv* init_floatv_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_floatv_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_floatv_dict(dict_fv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_floatv_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_floatv_dict(dict_fv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_floatv_dict 
* @brief Returns a float_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a float_v pointer so it can be used in vector and array functions
*/
float_v* return_floatv_pointer(dict_fv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_floatv_dict 
* @brief Returns a float_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_floatv_dict(dict_fv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_floatv_dict(dict_fv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_fv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define FDICTV_GBC __attribute__((cleanup(_free_floatv_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The float vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_floatv_dict(const dict_fv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The float vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type float_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_floatv_dict(dict_fv* dict, const char* key, float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t float_dictv_size(const dict_fv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t float_dictv_alloc(const dict_fv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t float_dictv_hash_size(const dict_fv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector float dictionary 
 *
 * @param original A float vector dictionary 
 * @return A copy of a dictionary
 */
dict_fv* copy_floatv_dict(const dict_fv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to float vector dictionaries
 *
 * Returns a merged float vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_fv* merge_floatv_dict(const dict_fv* dict1, const dict_fv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_floatv_dict(dict_fv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for float vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated float_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_fv_iterator)(const char* key, const float_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a float_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_floatv_dict(const dict_fv* dict, dict_fv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_floatv_dict(const dict_fv* dict);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a float container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any float container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - float_v (calls float_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to float container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define f_size(f_struct) _Generic((f_struct), \
    float_v*: float_vector_size, \
    dict_f*: float_dict_size, \
    dict_fv*: float_dictv_size) (f_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a float container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any float container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - float_v (calls float_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to float container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define f_alloc(f_struct) _Generic((f_struct), \
    float_v*: float_vector_alloc, \
    dict_f*: float_dict_alloc, \
    dict_fv*: float_dictv_alloc) (f_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_float_H */
// ================================================================================
// ================================================================================
// eof
