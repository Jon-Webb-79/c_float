**********
Dictionary
**********

Float Dictionary Overview
==========================

A float dictionary provides a hash table implementation for mapping string keys to float values, 
offering efficient key-value storage and retrieval. This implementation uses chained hashing for 
collision resolution and features automatic table resizing for optimal performance.

Key Features
------------

* Dynamic resizing: Automatic growth when load factor threshold is reached
* Efficient lookup: O(1) average case access time
* Memory safety: Proper encapsulation and memory management
* String key support: Automatic key duplication and management
* Collision handling: Chained hashing for robust collision resolution
* Automatic cleanup: Optional garbage collection support with FDICT_GBC

When to Use Float Dictionaries
------------------------------

Float dictionaries are particularly useful when:

* Mapping strings to numerical values
* Requiring fast key-value lookups
* Managing relationships between text identifiers and measurements
* Implementing caches or lookup tables with string keys
* Building numerical mapping tables
* Creating frequency counters for string data

Performance Characteristics
---------------------------

* Access time: O(1) average case for lookups and insertions
* Space efficiency: Adaptive growth strategy for memory efficiency
* Collision handling: Chained hashing for reliable performance under high load
* Memory overhead: Small per-entry overhead for key storage and chain pointers

Data Types
==========

The following are derived data types used in the implementation of float dictionaries.

dict_f
------
Opaque type representing a float dictionary that maps string keys to float values.
Implementation details are hidden from the user for encapsulation.

.. code-block:: c

   typedef struct dict_f dict_f;

Core Functions
==============

The float dictionary implementation provides a complete set of functions for dictionary 
manipulation. All functions are declared in the ``c_float.h`` header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section control the creation, memory allocation,
and destruction of ``dict_f`` data types.

init_float_dict
~~~~~~~~~~~~~~~~~
.. c:function:: dict_f* init_float_dict(void)

   Initializes a new empty dictionary with default initial capacity.

   :returns: Pointer to new dict_f object, or NULL on allocation failure
   :raises: Sets errno to ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      dict_f* dict = init_float_dict();
      if (!dict) {
          fprintf(stderr, "Failed to initialize dictionary\\n");
          return 1;
      }
      
      // Use dictionary...
      
      free_float_dict(dict);

free_float_dict
~~~~~~~~~~~~~~~
.. c:function:: void free_float_dict(dict_f* dict)

   Frees all memory associated with a dictionary, including all stored keys and nodes.
   
   :param dict: Dictionary to free
   
   Example:

   .. code-block:: c

      dict_f* dict = init_float_dict();
      // Use dictionary...
      free_float_dict(dict);
      dict = NULL;  // Good practice to avoid dangling pointer

FDICT_GBC
~~~~~~~~~
.. c:macro:: FDICT_GBC

   Macro that enables automatic cleanup of dictionaries when they go out of scope.
   Only available when using GCC or Clang compilers.

   Example:

   .. code-block:: c

      void process_data(void) {
          FDICT_GBC dict_f* dict = init_float_dict();
          // Use dictionary...
      }  // dict is automatically freed here

Data Insertion and Update
-------------------------

insert_float_dict
~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_float_dict(dict_f* dict, const char* key, float value)

   Inserts a new key-value pair into the dictionary. If the key already exists,
   the function returns false. The dictionary automatically resizes if needed.

   :param dict: Target dictionary
   :param key: String key to insert
   :param value: Float value to associate with key
   :returns: true if insertion successful, false if key exists or error occurs
   :raises: Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure,
           EEXIST if key already exists

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      
      if (insert_float_dict(dict, "temperature", 23.5f)) {
          printf("Value inserted successfully\n");
      }
      
      // Trying to insert same key again fails
      if (!insert_float_dict(dict, "temperature", 24.0f)) {
          printf("Key already exists\n");
      }

      printf("Key: 'temperature', Value: %f\n", get_float_dict_value(dict, "temperature"));

   .. code-block:: bash

      Value inserted succesfully 
      Key already exists
      Key: 'temperature', Value: 23.50000


update_float_dict
~~~~~~~~~~~~~~~~~
.. c:function:: bool update_float_dict(dict_f* dict, const char* key, float value)

   Updates the value associated with an existing key. If the key doesn't exist,
   the function returns false.

   :param dict: Target dictionary
   :param key: String key to update
   :param value: New float value to associate with key
   :returns: true if update successful, false if key not found or error occurs
   :raises: Sets errno to EINVAL for NULL inputs, ENOENT if key not found

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 31.7);
      insert_float_dict(dict, "pressure", 101.127);
      if (update_float_dict(dict, "temperature", 24.0f)) {
          printf("Value updated successfully\n");
      } else {
          printf("Key not found\n");
      }
      printf("Key: 'temperature', Value: %f\n", get_float_dict_value(dict, "temperature"));

   .. code-block:: bash 

      Value updated succesfully 
      Key: 'temperature', Value: 24.0000

Data Retrieval
--------------

get_float_dict_value
~~~~~~~~~~~~~~~~~~~~
.. c:function:: float get_float_dict_value(const dict_f* dict, const char* key)

   Retrieves the value associated with a key. Returns FLT_MAX if the key
   is not found.

   :param dict: Target dictionary
   :param key: String key to look up
   :returns: Associated float value, or FLT_MAX if not found
   :raises: Sets errno to EINVAL for NULL inputs, ENOENT if key not found

   Example:

   .. code-block:: c
      
      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 31.7);
      insert_float_dict(dict, "pressure", 101.127);
      if (update_float_dict(dict, "temperature", 24.0f)) {
          printf("Value updated successfully\n");
      } else {
          printf("Key not found\n");
      }
      printf("Key: 'temperature', Value: %f\n", get_float_dict_value(dict, "temperature"));

   .. code-block:: bash 

      Value updated succesfully 
      Key: 'temperature', Value: 24.0000
     
Data Removal
------------

pop_float_dict
~~~~~~~~~~~~~~
.. c:function:: float pop_float_dict(dict_f* dict, const char* key)

   Removes and returns the value associated with a key. Returns FLT_MAX if
   the key is not found.

   :param dict: Target dictionary
   :param key: String key to remove
   :returns: Value associated with key, or FLT_MAX if not found
   :raises: Sets errno to EINVAL for NULL inputs, ENOENT if key not found

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 31.7);
      insert_float_dict(dict, "temperature", 101.7);
      float value = pop_float_dict(dict, "temperature");
      float value = get_float_dict_value(dict, "temperature);
      if (value == FLT_MAX && errno = ENOENT) {
          printf("Removed value associated with: 'temperature'");
      }

   .. code-block::

      Removed value associated with 'temperature'

Utility Functions
-----------------

.. _float-dict-size-func:

float_dict_size
~~~~~~~~~~~~~~~
.. c:function:: size_t float_dict_size(const dict_f* dict)

  Returns the number of non-empty buckets in the dictionary.  The user 
  can also use the :ref:`f_size <f-size-macro>` Generic Macro in place 
  of this function.

  :param dict: Target dictionary
  :returns: Number of non-empty buckets, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example with single-item buckets:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     // Add values that will hash to different buckets
     insert_float_dict(dict, "temperature", 23.5f);
     insert_float_dict(dict, "humidity", 45.0f);
     insert_float_dict(dict, "pressure", 1013.2f);
     
     printf("Number of buckets used: %zu\n", float_dict_size(dict));
     // printf("Number of buckets used: %zu\n", f_size(dict) // Optional macro use
     printf("Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     free_float_dict(dict);

  Output::

     Number of buckets used: 3
     Total key-value pairs: 3

  Example with collision:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     // Add values that might hash to same bucket
     insert_float_dict(dict, "value1", 1.0f);
     insert_float_dict(dict, "value2", 2.0f);
     insert_float_dict(dict, "value3", 3.0f);
     
     printf("Number of buckets used: %zu\n", float_dict_size(dict));
     printf("Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     free_float_dict(dict);

  Output::

     Number of buckets used: 1
     Total key-value pairs: 3

.. _float-dict-alloc-func:

float_dict_alloc
~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dict_alloc(const dict_f* dict)

  Returns the total number of buckets allocated in the dictionary. The user 
  can also use the :ref:`f_alloc <f-alloc-macro>` Generic Macro in place 
  of this function. 

  :param dict: Target dictionary
  :returns: Total number of buckets, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example showing growth:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     printf("Initial allocation: %zu buckets\n", float_dict_alloc(dict));
     // printf("Initial allocations: %zy buckets\n", f_alloc(dict)) \\ Optional Macro use
     
     // Add many values to trigger resize
     char key[20];
     for(int i = 0; i < 20; i++) {
         sprintf(key, "key%d", i);
         insert_float_dict(dict, key, (float)i);
         
         if (i % 10 == 0) {
             printf("After %d insertions: %zu buckets\n", 
                    i+1, float_dict_alloc(dict));
         }
     }
     
     free_float_dict(dict);

  Output::

     Initial allocation: 16 buckets
     After 1 insertions: 16 buckets
     After 11 insertions: 32 buckets
     After 21 insertions: 32 buckets

float_dict_hash_size
~~~~~~~~~~~~~~~~~~~~
.. c:function:: size_t float_dict_hash_size(const dict_f* dict)

  Returns the total number of key-value pairs in the dictionary.

  :param dict: Target dictionary
  :returns: Number of key-value pairs, or SIZE_MAX on error
  :raises: Sets errno to EINVAL for NULL input

  Example showing relationship between metrics:

  .. code-block:: c

     dict_f* dict = init_float_dict();
     
     insert_float_dict(dict, "a", 1.0f);
     insert_float_dict(dict, "b", 2.0f);
     insert_float_dict(dict, "c", 3.0f);
     
     printf("Dictionary metrics:\n");
     printf("  Total buckets allocated: %zu\n", float_dict_alloc(dict));
     printf("  Buckets containing items: %zu\n", float_dict_size(dict));
     printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     // Remove one item
     pop_float_dict(dict, "b");
     
     printf("\nAfter removing one item:\n");
     printf("  Total buckets allocated: %zu\n", float_dict_alloc(dict));
     printf("  Buckets containing items: %zu\n", float_dict_size(dict));
     printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));
     
     free_float_dict(dict);

  Output::

     Dictionary metrics:
       Total buckets allocated: 16
       Buckets containing items: 3
       Total key-value pairs: 3

     After removing one item:
       Total buckets allocated: 16
       Buckets containing items: 2
       Total key-value pairs: 2

merge_float_dict
~~~~~~~~~~~~~~~~
.. c:function:: dict_f* merge_float_dict(const dict_f* dict1, const dict_f* dict2, bool overwrite)

   Merges two dictionaries into a new dictionary. The resulting dictionary contains all 
   entries from both input dictionaries. If a key exists in both dictionaries:

   - If ``overwrite`` is ``true``, the value from ``dict2`` replaces the value from ``dict1``.
   - If ``overwrite`` is ``false``, the original value from ``dict1`` is preserved.

   Neither ``dict1`` nor ``dict2`` is modified by this operation.

   :param dict1: First input dictionary
   :param dict2: Second input dictionary
   :param overwrite: If true, dict2 values overwrite dict1 values on key conflicts
   :returns: New dictionary containing merged entries, or NULL on failure
   :raises: Sets errno to EINVAL for NULL inputs, or propagates errors from underlying operations

   Example:

   .. code-block:: c

      dict_f* dict1 FDICT_GBC = init_float_dict();
      dict_f* dict2 FDICT_GBC = init_float_dict();
      
      insert_float_dict(dict1, "temperature", 25.0f);
      insert_float_dict(dict1, "humidity", 40.0f);

      insert_float_dict(dict2, "humidity", 45.0f);  // Key conflict
      insert_float_dict(dict2, "pressure", 1012.5f);

      // Merge with overwrite
      dict_f* merged FDICT_GBC = merge_float_dict(dict1, dict2, true);

      printf("Merged Dictionary:\n");
      foreach_float_dict(merged, print_entry, NULL);

   Example output::

      Merged Dictionary:
      temperature: 25.00
      humidity: 45.00
      pressure: 1012.50

   Example without overwrite:

   .. code-block:: c

      dict_f* merged_no_overwrite FDICT_GBC = merge_float_dict(dict1, dict2, false);

      printf("Merged Dictionary (no overwrite):\n");
      foreach_float_dict(merged_no_overwrite, print_entry, NULL);

   Example output::

      Merged Dictionary (no overwrite):
      temperature: 25.00
      humidity: 40.00
      pressure: 1012.50

   Notes:

   - The caller is responsible for freeing the returned merged dictionary.
   - If memory allocation fails at any point, NULL is returned and errno is set appropriately.

clear_float_dict
~~~~~~~~~~~~~~~~
.. c:function:: bool clear_float_dict(dict_f* dict)

   Removes all key-value pairs from the dictionary without freeing the dictionary itself.
   After calling this function, the dictionary remains allocated and can be reused
   without reinitialization.

   :param dict: Target dictionary to clear
   :returns: true if all entries were cleared successfully, false otherwise
   :raises: Sets errno to EINVAL for NULL input

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      
      insert_float_dict(dict, "temperature", 23.5f);
      insert_float_dict(dict, "pressure", 1013.2f);

      printf("Before clearing:\n");
      printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));
      
      clear_float_dict(dict);

      printf("After clearing:\n");
      printf("  Total key-value pairs: %zu\n", float_dict_hash_size(dict));

   Output::

      Before clearing:
        Total key-value pairs: 2
      After clearing:
        Total key-value pairs: 0

   Notes:

   - The dictionary structure and its internal hash table remain allocated after clearing.
   - This function is useful when reusing an existing dictionary without reallocating it.

copy_float_dict
~~~~~~~~~~~~~~~
.. c:function:: dict_f* copy_float_dict(const dict_f* dict)

   Creates a deep copy of a dictionary, duplicating all key-value pairs into a new dictionary.
   Changes made to the copied dictionary do not affect the original.

   :param dict: Target dictionary to copy
   :returns: Pointer to new dictionary containing copies of all entries, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, or ENOMEM for allocation failure

   Example:

   .. code-block:: c

      dict_f* original FDICT_GBC = init_float_dict();
      insert_float_dict(original, "sensor1", 10.5f);
      insert_float_dict(original, "sensor2", 12.3f);

      dict_f* duplicate FDICT_GBC = copy_float_dict(original);

      printf("Original Dictionary:\n");
      foreach_float_dict(original, print_entry, NULL);

      printf("\nCopied Dictionary:\n");
      foreach_float_dict(duplicate, print_entry, NULL);

      // Modify original
      update_float_dict(original, "sensor1", 99.9f);

      printf("\nAfter modifying original:\n");
      printf("Original Dictionary:\n");
      foreach_float_dict(original, print_entry, NULL);
      printf("Copied Dictionary (unchanged):\n");
      foreach_float_dict(duplicate, print_entry, NULL);

   Output::

      Original Dictionary:
      sensor1: 10.50
      sensor2: 12.30

      Copied Dictionary:
      sensor1: 10.50
      sensor2: 12.30

      After modifying original:
      Original Dictionary:
      sensor1: 99.90
      sensor2: 12.30
      Copied Dictionary (unchanged):
      sensor1: 10.50
      sensor2: 12.30

   Notes:

   - The caller is responsible for freeing the copied dictionary using `free_float_dict`.
   - Copying a NULL dictionary returns NULL and sets errno to EINVAL.

has_key_float_dict
~~~~~~~~~~~~~~~~~~
.. c:function:: bool has_key_float_dict(const dict_f* dict, const char* key)

   Checks if a specified key exists in the dictionary without retrieving its value.

   :param dict: Target dictionary to search
   :param key: String key to look for
   :returns: true if key exists, false otherwise
   :raises: Sets errno to EINVAL for NULL input

   Example:

   .. code-block:: c

      dict_f* dict FDICT_GBC = init_float_dict();
      insert_float_dict(dict, "temperature", 23.5f);
      insert_float_dict(dict, "pressure", 1012.8f);

      if (has_key_float_dict(dict, "temperature")) {
          printf("'temperature' exists in the dictionary\n");
      } else {
          printf("'temperature' not found\n");
      }

      if (!has_key_float_dict(dict, "humidity")) {
          printf("'humidity' not found in the dictionary\n");
      }

   Output::

      'temperature' exists in the dictionary
      'humidity' not found in the dictionary

   Notes:

   - This function does not modify the dictionary.
   - Useful for checking the presence of a key before inserting or updating.


Iterator Support
----------------

dict_iterator
~~~~~~~~~~~~~
.. c:type:: void (*dict_iterator)(const char* key, float value, void* user_data)

   Function type for dictionary iteration callbacks.

   :param key: Current key being visited
   :param value: Value associated with current key
   :param user_data: User-provided context data

foreach_float_dict
~~~~~~~~~~~~~~~~~~
.. c:function:: bool foreach_float_dict(const dict_f* dict, dict_iterator iter, void* user_data)

   Iterates over all key-value pairs in the dictionary, calling the provided
   callback function for each pair.

   :param dict: Target dictionary
   :param iter: Iterator callback function
   :param user_data: Optional user data passed to callback
   :returns: true if iteration completed, false on error
   :raises: Sets errno to EINVAL for NULL dict or iter

   Example:

  .. code-block:: c

     // Basic print callback
     void print_entry(const char* key, float value, void* user_data) {
         printf("%s: %.2f\n", key, value);
     }

     // Create and populate dictionary
     dict_f* dict = init_float_dict();
     insert_float_dict(dict, "temperature", 23.5f);
     insert_float_dict(dict, "humidity", 45.0f);
     insert_float_dict(dict, "pressure", 1013.2f);

     printf("Dictionary contents:\n");
     foreach_float_dict(dict, print_entry, NULL);

     free_float_dict(dict);

  Output::

     Dictionary contents:
     temperature: 23.50
     humidity: 45.00
     pressure: 1013.20

  Example with data accumulation:

  .. code-block:: c

     // Structure for accumulating statistics
     typedef struct {
         float sum;
         size_t count;
         float min;
         float max;
     } stats_data;

     // Callback to gather statistics
     void gather_stats(const char* key, float value, void* user_data) {
         stats_data* stats = (stats_data*)user_data;
         stats->sum += value;
         stats->count++;
         if (value < stats->min) stats->min = value;
         if (value > stats->max) stats->max = value;
     }

     // Print callback with formatted output
     void print_with_prefix(const char* key, float value, void* user_data) {
         const char* prefix = (const char*)user_data;
         printf("%s%s: %.2f\n", prefix, key, value);
     }

     int main() {
         dict_f* dict = init_float_dict();
         
         // Add some sensor readings
         insert_float_dict(dict, "sensor1", 10.5f);
         insert_float_dict(dict, "sensor2", 15.7f);
         insert_float_dict(dict, "sensor3", 12.3f);
         insert_float_dict(dict, "sensor4", 8.9f);
         
         // Print with custom prefix
         printf("Raw Readings:\n");
         const char* prefix = "  Reading ";
         foreach_float_dict(dict, print_with_prefix, (void*)prefix);
         
         // Calculate statistics
         stats_data stats = {0.0f, 0, FLT_MAX, -FLT_MAX};
         foreach_float_dict(dict, gather_stats, &stats);
         
         printf("\nStatistics:\n");
         printf("  Number of readings: %zu\n", stats.count);
         printf("  Average reading: %.2f\n", stats.sum / stats.count);
         printf("  Minimum reading: %.2f\n", stats.min);
         printf("  Maximum reading: %.2f\n", stats.max);
         
         free_float_dict(dict);
         return 0;
     }

  Output::

     Raw Readings:
       Reading sensor1: 10.50
       Reading sensor2: 15.70
       Reading sensor3: 12.30
       Reading sensor4: 8.90

     Statistics:
       Number of readings: 4
       Average reading: 11.85
       Minimum reading: 8.90
       Maximum reading: 15.70

  Example with error handling:

  .. code-block:: c

     bool iterate_dict(dict_f* dict, dict_iterator iter, void* user_data) {
         if (!foreach_float_dict(dict, iter, user_data)) {
             if (errno == EINVAL) {
                 printf("Error: Invalid dictionary or iterator\n");
             } else {
                 printf("Unknown error during iteration\n");
             }
             return false;
         }
         return true;
     }

     // Test error cases
     dict_f* dict = init_float_dict();
     insert_float_dict(dict, "test", 1.0f);

     printf("Testing NULL dictionary:\n");
     iterate_dict(NULL, print_entry, NULL);

     printf("\nTesting NULL iterator:\n");
     iterate_dict(dict, NULL, NULL);

     free_float_dict(dict);

  Output::

     Testing NULL dictionary:
     Error: Invalid dictionary or iterator

     Testing NULL iterator:
     Error: Invalid dictionary or iterator

Key/Value Extraction
--------------------

get_keys_float_dict
~~~~~~~~~~~~~~~~~~~
.. c:function:: string_v* get_keys_float_dict(const dict_f* dict)

   Returns a ``string_v`` object containing all keys in the dictionary.  The developer 
   must use the ``c_string.h`` header file with this function.  A full description 
   of the ``string_v`` object can be found at `C String Library <https://c-string.readthedocs.io/en/latest/>`_ 

   :param dict: Target dictionary
   :returns: Vector containing all keys, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, ENOMEM for allocation failure

   .. code-block:: c
    
      #include "c_float.h"
      #include "c_string.h"


      dict_f* dict = init_float_dict();
      insert_float_dict("One", 1.1f);
      insert_float_dict("Two", 2.2f);
      insert_float_dict("Three", 3.3f);
      insert_float_dict("Four", 4.4f);
      string_v* values = get_keys_float_dict(dict);

      printf("Vector has %zu indices", s_size(values));
      printf("[ ");
      for (size_t i = 1; i < s_size(values) - 1; i++) {
          printf("%f, ", str_vector_index(vec, i));
      }
      printf("%f ]", str_vector_index(vec, s_size(values)));

      free_float_dict(dict);
      free_str_vector(dict);

   .. code-block::

      Vector has 4 indices
      [ One, Two, Three, Four ]

get_values_float_dict
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float_v* get_values_float_dict(const dict_f* dict)

   Returns a ``float_v`` object containing all values in the dictionary as a 
   dynamically allocated vector.
   The user should consult with the :ref:`Float Vector <vector_file>` documentation
   to understand how to utilizie the ``float_v`` object and how to properly 
   free all vector memory.  The ``float_v`` object is contained within the 
   context of the ``c_float.h`` header file

   :param dict: Target dictionary
   :returns: Vector containing all values, or NULL on error
   :raises: Sets errno to EINVAL for NULL input, ENOMEM for allocation failure

   Example:

   .. code-block:: c

      dict_f* dict = init_float_dict();
      insert_float_dict("One", 1.1f);
      insert_float_dict("Two", 2.2f);
      insert_float_dict("Three", 3.3f);
      insert_float_dict("Four", 4.4f);
      float_v* values = get_values_float_dict(dict);

      printf("Vector has %zu indices", f_size(values));
      printf("[ ");
      for (size_t i = 1; i < f_size(values) - 1; i++) {
          printf("%f, ", float_vector_index(vec, i));
      }
      printf("%f ]", float_vector_index(vec, f_size(values)));

      free_float_dict(dict);
      free_float_vector(dict);

   .. code-block::

      Vector has 4 indices
      [ 1.10000, 2.20000, 3.30000, 4.40000 ]
