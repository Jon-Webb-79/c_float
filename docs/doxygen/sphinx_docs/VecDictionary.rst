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

