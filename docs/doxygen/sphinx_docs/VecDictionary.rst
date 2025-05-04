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
