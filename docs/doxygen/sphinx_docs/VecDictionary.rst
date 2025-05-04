*****************
Vector Dictionary
*****************

Float Vector Dictionary Overview
================================

A float vector dictionary (`dict_fv`) maps string keys to dynamically allocated float vector (`float_v`) values. 
It supports efficient retrieval and manipulation of float arrays via their associated keys, 
with support for automatic memory management, key-based lookup, and vector pointer access.

Key Features
------------

* Maps strings to dynamically allocated float vectors (`float_v`)
* Pointer access to vectors for compatibility with all `float_v` functions
* Automatic resizing based on load factor
* Clean separation of memory ownership and safety
* Optional automatic cleanup using `FDICTV_GBC`

When to Use Float Vector Dictionaries
-------------------------------------

* Managing named arrays of numerical data (e.g., sensor histories, grouped data)
* Passing vector references into numerical and statistical functions
* Implementing high-performance lookup tables for float vectors
* Maintaining structured mappings of string labels to float arrays

Performance Characteristics
---------------------------

* Lookup and insert: O(1) average time using chained hashing
* Optimized for dynamic arrays only — `STATIC` arrays are not allowed
* Supports full dictionary and vector lifecycle management

Data Types
==========

The following are derived data types used in the implementation of float dictionaries.

dict_fv
-------
Opaque type representing a float vector dictionary that maps string keys to 
`float_v*` values.

.. code-block:: c

   typedef struct dict_fv dict_fv;

   // Do not access internal structure directly — use provided API

Core Functions
==============

The float vector dictionary implementation provides a complete set of functions 
for dictionary manipulation. All functions are declared in the ``c_float.h`` 
header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section control the creation, memory allocation,
and destruction of ``dict_fv`` data types.

init_floatv_dict
~~~~~~~~~~~~~~~~
.. c:function:: dict_f* init_floatv_dict(void)

   Initializes a new empty dictionary with default initial capacity.

   :returns: Pointer to new dict_fv object, or NULL on allocation failure
   :raises: Sets errno to ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      if (!dict) {
          fprintf(stderr, "Failed to initialize dictionary\\n");
          return 1;
      }
      
      // Use dictionary...
      
      free_floatv_dict(dict);

free_floatv_dict
~~~~~~~~~~~~~~~~
.. c:function:: void free_floatv_dict(dict_f* dict)

   Frees all memory associated with a dictionary, including all stored keys and nodes.
   
   :param dict: Dictionary to free
   
   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      // Use dictionary...
      free_floatv_dict(dict);
      dict = NULL;  // Good practice to avoid dangling pointer

FDICTV_GBC
~~~~~~~~~~
.. c:macro:: FDICTV_GBC

   Macro that enables automatic cleanup of dictionaries when they go out of scope.
   Only available when using GCC or Clang compilers.

   Example:

   .. code-block:: c

      void process_data(void) {
          FDICTV_GBC dict_f* dict = init_floatv_dict();
          // Use dictionary...
      }  // dict is automatically freed here

Data Insertion and Update
-------------------------
The following functions can be used to insert vectors to the ``dict_fv`` data 
type.

create_floatv_dict 
~~~~~~~~~~~~~~~~~~
.. c:function:: bool create_floatv_dict(dict_fv* dict, char* key, size_t size)

   Creates a new dynamically allocated vector within the dictionary and assigns 
   it to a string literal key.

   :param dict: Target dictionary
   :param key: String key to insert
   :param size: An estimate for the size of the dynamically allocated array
   :returns: true if insertion successful, false if key exists or error occurs
   :raises: Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure,
            EEXIST if key already exists

   Example:

   .. code-block:: c

      dict_f* dict FDICTV_GBC = init_floatv_dict();
      
      if (create_floatv_dict(dict, "temperature", 20)) {
          printf("Array of size %d assigned to the key '%s'\n", 20, "temperature");
      }
     
   .. code-block:: bash

      Array of size 20 assigned to the key 'temperature'

insert_floatv_dict
~~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_floatv_dict(dict_fv* dict, const char* key, float_v* vec)

   Inserts an existing dynamically allocated float vector into the dictionary under the specified key.
   This function allows advanced users to create and manipulate `float_v` objects independently before 
   assigning them to a key-value pair in the dictionary.

   Unlike :c:func:`create_floatv_dict`, which allocates a new vector internally, this function
   accepts ownership of an already allocated dynamic vector and associates it with the provided key.
   Once this dictionary assumes ownership of the vector, you should only handle data 
   within the vector through this dictionary to avoid memory issues.

   :param dict: Target dictionary
   :param key: String key to associate with the float vector
   :param vec: A dynamically allocated float vector (`float_v*`) to insert
   :returns: true on successful insertion, false on failure
   :raises: 
     - `EINVAL` if any argument is NULL  
     - `EEXIST` if the key already exists in the dictionary  
     - `EPERM` if the float vector was not dynamically allocated  
     - `ENOMEM` if internal memory allocation fails

   .. important::

      This function only accepts dynamically allocated float vectors. 
      Attempting to insert a statically allocated vector will fail with `errno` set to `EPERM`.

   Example:

   .. code-block:: c

      dict_fv* dict FDICTV_GBC = init_floatv_dict();
      
      float_v* vec = init_float_vector(10);
      for (int i = 0; i < 10; i++) {
          push_back_float_vector(vec, (float)i);
      }

      if (!insert_floatv_dict(dict, "acceleration", vec)) {
          perror("Insert failed");
          free_float_vector(vec);  // Must be freed manually if insert fails
      }

      // Use dictionary...

   Example Output:

   .. code-block:: bash

      (no output if success; if failure due to static vector:)
      Insert failed: Operation not permitted

   Notes:

   - This function does not duplicate the vector — ownership of `vec` is transferred to the dictionary.
   - The dictionary will automatically free the vector when it is removed or when the dictionary is destroyed.
   - Use this function when the vector has been created and populated prior to insertion.

Data Retrieval
--------------
These functions allow a user to access data from ``dict_fv`` data types.

return_floatv_pointer
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float_v* return_floatv_pointer(dict_fv* dict, const char* key)

   Retrieves a pointer to the float vector associated with a given key.

   This function enables direct access to the underlying `float_v` object for use
   in numerical operations or vector manipulation routines. It is particularly
   useful for passing the vector to other library functions that operate on
   `float_v*` types.

   :param dict: Target dictionary
   :param key: Key string whose associated vector should be returned
   :returns: Pointer to the associated `float_v` object, or NULL on error
   :raises: 
     - `EINVAL` if either `dict` or `key` is NULL  
     - `ENOENT` if the key is not found in the dictionary

   Example:

   .. code-block:: c

      dict_fv* dict FDICTV_GBC = init_floatv_dict();
      create_floatv_dict(dict, "velocity", 10);
      float_v* v = return_floatv_pointer(dict, "velocity");

      if (!v) {
          perror("Failed to retrieve vector");
          return 1;
      }

      push_back_float_vector(v, 12.34f);
      printf("velocity[0] = %f\n", float_vector_index(v, 0));

   Output:

   .. code-block:: bash

      velocity[0] = 12.340000

   Notes:

   - The returned pointer is owned by the dictionary — do not free it manually.
   - Always check the return value for NULL before using the pointer.
   - If the key does not exist, `errno` is set to `ENOENT` and NULL is returned.

Data Removal
------------

pop_floatv_dict
~~~~~~~~~~~~~~~
.. c:function:: bool pop_floatv_dict(dict_fv* dict, const char* key)

   Removes a key-vector pair from the dictionary and deallocates the associated vector.

   This function deletes the `float_v` vector associated with the given key and removes
   the key from the hash table. It is a safe way to free memory associated with specific
   entries without affecting the rest of the dictionary.

   :param dict: Target dictionary
   :param key: Key string to remove
   :returns: true if the entry was found and removed, false otherwise
   :raises: 
     - `EINVAL` if `dict` or `key` is NULL  
     - `ENOENT` if the key does not exist in the dictionary

   Example:

   .. code-block:: c

      dict_fv* dict FDICTV_GBC = init_floatv_dict();
      create_floatv_dict(dict, "data", 5);

      // Add some values
      float_v* v = return_floatv_pointer(dict, "data");
      push_back_float_vector(v, 3.14f);
      push_back_float_vector(v, 2.71f);

      // Remove the entry
      if (pop_floatv_dict(dict, "data")) {
          printf("Key 'data' and associated vector removed successfully\n");
      }

   Output:

   .. code-block:: bash

      Key 'data' and associated vector removed successfully

   Notes:

   - If the key is not found, the function returns false and sets `errno` to `ENOENT`.
   - The memory for both the vector and key string is freed automatically.
   - After removal, attempting to access the key again will result in an error.

Utility Functions
-----------------

.. _floatv-dict-size-func:

float_dictv_size
~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dictv_size(const dict_fv* dict)

  Returns the number of non-empty buckets in the float vector dictionary.  
  The user can also use the :ref:`f_size <f-size-macro>` Generic Macro 
  in place of this function.

  :param dict: Target float vector dictionary
  :returns: Number of non-empty buckets, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example with distinct keys:

  .. code-block:: c

     dict_fv* dict = init_floatv_dict();

     create_floatv_dict(dict, "temperature", 10);
     create_floatv_dict(dict, "humidity", 5);
     create_floatv_dict(dict, "pressure", 3);

     printf("Number of buckets used: %zu\n", float_dictv_size(dict));
     printf("Total key-value pairs: %zu\n", float_dictv_hash_size(dict));

     free_floatv_dict(dict);

  Output::

     Number of buckets used: 3
     Total key-value pairs: 3

  Example with hash collisions:

  .. code-block:: c

     dict_fv* dict = init_floatv_dict();

     create_floatv_dict(dict, "aaa", 1);
     create_floatv_dict(dict, "bbb", 1);
     create_floatv_dict(dict, "ccc", 1); // These might hash to the same bucket

     printf("Number of buckets used: %zu\n", float_dictv_size(dict));
     printf("Total key-value pairs: %zu\n", float_dictv_hash_size(dict));

     free_floatv_dict(dict);

  Output::

     Number of buckets used: 1
     Total key-value pairs: 3

.. _floatv-dict-alloc-func:

float_dictv_alloc 
~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dictv_alloc(const dict_fv* dict)

Returns the total number of hash buckets allocated in the float vector dictionary.
The user can also use the :ref:f_alloc <f-alloc-macro> Generic Macro
in place of this function.

:param dict: Target float vector dictionary
:returns: Total number of buckets, or SIZE_MAX on error
:raises: Sets errno to EINVAL for NULL input

Example showing automatic resizing:

.. code-block:: c 

    dict_fv* dict = init_floatv_dict();

    printf("Initial allocation: %zu buckets\n", float_dictv_alloc(dict));
    // printf("Initial allocation: %zu buckets\n", f_alloc(dict)); // Optional macro use

    char key[20];
    for (int i = 0; i < 30; ++i) {
        sprintf(key, "key%d", i);
        create_floatv_dict(dict, key, 4);

        if (i % 10 == 0) {
            printf("After %d insertions: %zu buckets\n", 
                   i + 1, float_dictv_alloc(dict));
        }
    }

    free_floatv_dict(dict);

Output:: 

   Initial allocation: 16 buckets
   After 1 insertions: 16 buckets
   After 11 insertions: 32 buckets
   After 21 insertions: 48 buckets
  
float_dictv_hash_size 
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dictv_hash_size(const dict_fv* dict)

  Returns the total number of key-vector pairs in the dictionary.

  :param dict: Target float vector dictionary
  :returns: Number of key-value pairs, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example with additions and deletions:

  .. code-block:: c

     dict_fv* dict = init_floatv_dict();

     create_floatv_dict(dict, "sensor1", 5);
     create_floatv_dict(dict, "sensor2", 5);
     create_floatv_dict(dict, "sensor3", 5);

     printf("Initial metrics:\n");
     printf("  Total allocated: %zu\n", float_dictv_alloc(dict));
     printf("  Buckets used:    %zu\n", float_dictv_size(dict));
     printf("  Total entries:   %zu\n", float_dictv_hash_size(dict));

     pop_floatv_dict(dict, "sensor2");

     printf("\nAfter removal:\n");
     printf("  Total allocated: %zu\n", float_dictv_alloc(dict));
     printf("  Buckets used:    %zu\n", float_dictv_size(dict));
     printf("  Total entries:   %zu\n", float_dictv_hash_size(dict));

     free_floatv_dict(dict);

  Output::

     Initial metrics:
       Total allocated: 16
       Buckets used:    3
       Total entries:   3

     After removal:
       Total allocated: 16
       Buckets used:    2
       Total entries:   2

merge_floatv_dict
~~~~~~~~~~~~~~~~~
.. c:function:: dict_fv* merge_floatv_dict(const dict_fv* dict1, const dict_fv* dict2, bool overwrite)

   Merges two float vector dictionaries into a new dictionary. The resulting dictionary 
   contains all key-vector pairs from both input dictionaries. If a key exists in both:

   - If ``overwrite`` is ``true``, the vector from ``dict2`` replaces the vector from ``dict1``.
   - If ``overwrite`` is ``false``, the vector from ``dict1`` is preserved.

   All vectors are deep copied. Neither input dictionary is modified.

   :param dict1: First dictionary to merge
   :param dict2: Second dictionary to merge
   :param overwrite: Determines whether values from ``dict2`` replace existing keys in ``dict1``
   :returns: Newly allocated merged dictionary, or NULL on error
   :raises: Sets errno to EINVAL if inputs are NULL, EPERM if non-dynamic vectors are found, or ENOMEM

   Example:

   .. code-block:: c

      dict_fv* dict1 = init_floatv_dict();
      dict_fv* dict2 = init_floatv_dict();

      float_v* vec1 = init_float_vector(1);
      float_v* vec2 = init_float_vector(1);
      push_back_float_vector(vec1, 3.14f);
      push_back_float_vector(vec2, 6.28f);

      insert_floatv_dict(dict1, "value", vec1);
      insert_floatv_dict(dict2, "value", vec2);

      dict_fv* merged = merge_floatv_dict(dict1, dict2, true);

      float_v* result = return_floatv_pointer(merged, "value");
      printf("Merged value: %.2f\n", float_vector_index(result, 0));

      free_floatv_dict(dict1);
      free_floatv_dict(dict2);
      free_floatv_dict(merged);

   Output::

      Merged value: 6.28

   Notes:

   - This function guarantees deep copies of all vectors.
   - Caller is responsible for freeing the merged dictionary.
   - All vectors must be dynamically allocated.

clear_floatv_dict
~~~~~~~~~~~~~~~~~
.. c:function:: void clear_floatv_dict(dict_fv* dict)

   Removes all key-vector pairs from the dictionary without deallocating the dictionary structure itself.
   This operation is useful when reusing the same dictionary after clearing its contents.

   :param dict: Dictionary to clear
   :raises: Sets errno to EINVAL if the input is NULL

   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      create_floatv_dict(dict, "temperature", 3);
      create_floatv_dict(dict, "pressure", 2);

      printf("Before clear: %zu keys\n", float_dictv_hash_size(dict));

      clear_floatv_dict(dict);

      printf("After clear: %zu keys\n", float_dictv_hash_size(dict));

      free_floatv_dict(dict);

   Output::

      Before clear: 2 keys
      After clear: 0 keys

   Notes:

   - All vectors and keys are freed.
   - The dictionary is reusable after this operation.

copy_floatv_dict
~~~~~~~~~~~~~~~~
.. c:function:: dict_fv* copy_floatv_dict(const dict_fv* original)

   Creates a deep copy of a float vector dictionary. Each vector in the copy is a
   newly allocated clone of the original vector.

   :param original: Dictionary to copy
   :returns: A newly allocated dictionary containing deep copies of all entries
   :raises: Sets errno to EINVAL if input is NULL, or ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      dict_fv* original = init_floatv_dict();
      float_v* vec = init_float_vector(1);
      push_back_float_vector(vec, 9.81f);
      insert_floatv_dict(original, "gravity", vec);

      dict_fv* copy = copy_floatv_dict(original);

      float_v* copied_vec = return_floatv_pointer(copy, "gravity");
      printf("Copied value: %.2f\n", float_vector_index(copied_vec, 0));

      free_floatv_dict(original);
      free_floatv_dict(copy);

   Output::

      Copied value: 9.81

   Notes:

   - The copied dictionary is independent of the original.
   - Useful for isolating dictionary state across function calls or processing steps.

has_key_floatv_dict
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool has_key_floatv_dict(const dict_fv* dict, const char* key)

   Checks if a given key exists in the float vector dictionary.

   :param dict: Dictionary to query
   :param key: Key string to look for
   :returns: true if key exists, false otherwise
   :raises: Sets errno to EINVAL for NULL inputs

   Example:

   .. code-block:: c

      dict_fv* dict = init_floatv_dict();
      create_floatv_dict(dict, "velocity", 10);

      if (has_key_floatv_dict(dict, "velocity")) {
          printf("Found 'velocity'\n");
      }

      if (!has_key_floatv_dict(dict, "altitude")) {
          printf("'altitude' not found\n");
      }

      free_floatv_dict(dict);

   Output::

      Found 'velocity'
      'altitude' not found

   Notes:

   - This function does not retrieve or modify the vector.
   - Can be used to check for key presence before insertion or access.

Iterator Support
----------------

dict_fv_iterator
~~~~~~~~~~~~~~~~
.. c:type:: void (*dict_fv_iterator)(const char* key, const float_v* value, void* user_data)

   Function type for float vector dictionary iteration callbacks.

   :param key: Current key being visited
   :param value: Pointer to the associated float_v vector
   :param user_data: Optional user-provided context data

foreach_floatv_dict
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool foreach_floatv_dict(const dict_fv* dict, dict_fv_iterator iter, void* user_data)

   Iterates over all key-vector pairs in the dictionary, calling the provided
   callback function for each entry.

   :param dict: Target float vector dictionary
   :param iter: Iterator callback function
   :param user_data: Optional user context data passed to the callback
   :returns: true if iteration completes successfully, false on error
   :raises: Sets errno to EINVAL if dict or iter is NULL

   Example:

   .. code-block:: c

      void print_vector_entry(const char* key, const float_v* vec, void* user_data) {
          printf("%s: [", key);
          for (size_t i = 0; i < f_size(vec); ++i) {
              printf("%.2f%s", float_vector_index(vec, i), (i < f_size(vec) - 1) ? ", " : "");
          }
          printf("]\n");
      }

      dict_fv* dict = init_floatv_dict();
      float_v* vec1 = init_float_vector(3);
      push_back_float_vector(vec1, 1.1f);
      push_back_float_vector(vec1, 2.2f);
      push_back_float_vector(vec1, 3.3f);
      insert_floatv_dict(dict, "set1", vec1);

      float_v* vec2 = init_float_vector(2);
      push_back_float_vector(vec2, 4.4f);
      push_back_float_vector(vec2, 5.5f);
      insert_floatv_dict(dict, "set2", vec2);

      printf("Dictionary contents:\n");
      foreach_floatv_dict(dict, print_vector_entry, NULL);
      free_floatv_dict(dict);

   Output::

      Dictionary contents:
      set1: [1.10, 2.20, 3.30]
      set2: [4.40, 5.50]

   Example with data aggregation:

   .. code-block:: c

      typedef struct {
          float total_sum;
          size_t total_count;
      } agg_data;

      void sum_elements(const char* key, const float_v* vec, void* user_data) {
          agg_data* stats = (agg_data*)user_data;
          for (size_t i = 0; i < f_size(vec); ++i) {
              stats->total_sum += float_vector_index(vec, i);
              stats->total_count++;
          }
      }

      dict_fv* dict = init_floatv_dict();
      float_v* vec = init_float_vector(4);
      for (float i = 0; i < 4.0f; i++)
          push_back_float_vector(vec, i);
      insert_floatv_dict(dict, "numbers", vec);

      agg_data stats = {0};
      foreach_floatv_dict(dict, sum_elements, &stats);
      printf("Sum: %.2f, Count: %zu\n", stats.total_sum, stats.total_count);
      free_floatv_dict(dict);

   Output::

      Sum: 6.00, Count: 4

   Example with error handling:

   .. code-block:: c

      bool safe_iteration(dict_fv* dict, dict_fv_iterator iter, void* user_data) {
          if (!foreach_floatv_dict(dict, iter, user_data)) {
              if (errno == EINVAL) {
                  fprintf(stderr, "Invalid dictionary or iterator\n");
              }
              return false;
          }
          return true;
      }

      void dummy_iter(const char* key, const float_v* vec, void* user_data) {}

      int main(void) {
          dict_fv* dict = init_floatv_dict();
          insert_floatv_dict(dict, "test", init_float_vector(1));

          safe_iteration(NULL, dummy_iter, NULL);  // NULL dictionary
          safe_iteration(dict, NULL, NULL);        // NULL iterator

          free_floatv_dict(dict);
          return 0;
      }

   Output::

      Invalid dictionary or iterator
      Invalid dictionary or iterator

   Notes:

   - The callback function is invoked once for each key-vector pair.
   - The `user_data` parameter can be used to accumulate statistics or context.

get_keys_floatv_dict
~~~~~~~~~~~~~~~~~~~~
.. c:function:: string_v* get_keys_floatv_dict(const dict_fv* dict)

   Returns a ``string_v`` object containing all keys from a float vector dictionary.
   Each key maps to a dynamically allocated ``float_v`` array in the original dictionary.

   The developer must include the ``c_string.h`` header file to use the ``string_v`` type.
   A full description of the ``string_v`` object can be found at 
   `C String Library <https://c-string.readthedocs.io/en/latest/>`_.

   :param dict: Target float vector dictionary
   :returns: Vector containing all keys, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, or ENOMEM for memory allocation failure

   Example:

   .. code-block:: c

      #include "c_float.h"
      #include "c_string.h"

      dict_fv* dict = init_floatv_dict();
      create_floatv_dict(dict, "One", 3);
      create_floatv_dict(dict, "Two", 3);
      create_floatv_dict(dict, "Three", 3);
      create_floatv_dict(dict, "Four", 3);

      string_v* keys = get_keys_floatv_dict(dict);

      printf("Vector has %zu indices\n", s_size(keys));
      printf("[ ");
      for (size_t i = 0; i < s_size(keys) - 1; i++) {
          printf("%s, ", str_vector_index(keys, i));
      }
      printf("%s ]\n", str_vector_index(keys, s_size(keys) - 1));

      free_floatv_dict(dict);
      free_str_vector(keys);

   Example output::

      Vector has 4 indices
      [ One, Two, Three, Four ]

