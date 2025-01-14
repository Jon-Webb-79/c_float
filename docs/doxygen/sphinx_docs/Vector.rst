******
Vector
******

Float Vector Overview
=====================

A float vector provides both dynamic and static array containers for float values, offering
efficient management of numerical data with automatic memory handling for dynamic arrays
and safe access for static arrays. This implementation combines the flexibility of dynamic
arrays with the safety of bounds checking and proper memory management.

Key Features
------------

* Dynamic and static array support: Choose between automatic resizing or fixed-size arrays
* Memory safety: Proper encapsulation and memory management
* Bounds checking: Safe access to array elements
* Efficient access: O(1) access to any float in the vector
* Automatic cleanup: Optional garbage collection support with FLTVEC_GBC

When to Use Float Vectors
-------------------------

Float vectors are particularly useful when:

* Managing collections of float values that may grow or shrink
* Working with an unknown number of float values at compile time
* Requiring sequential access to multiple float values
* Needing efficient random access to float values by index
* Managing related numerical data as a single unit
* Implementing numerical algorithms or data processing pipelines

Performance Characteristics
---------------------------

* Access time: O(1) for index-based access
* Insertion time: O(1) amortized for push_back operations
* Memory efficiency: Dynamic vectors grow geometrically to minimize reallocations
* Memory overhead: Minimal per-element overhead

Data Types
==========

The following are derived data types used in the implementation of float vectors.

float_v
-------
The float vector type that can manage either dynamically or statically allocated float arrays
with automatic bounds checking and memory management.

.. code-block:: c

   typedef struct {
       float* data;
       size_t len;
       size_t alloc;
       alloc_t alloc_type;
   } float_v;

Core Functions
==============

The c_float library provides both dynamically and statically allocated float arrays through
the float_v type. All of the functions described in this section can be found in the
``c_float.h`` header file.

Initialization and Memory Management
------------------------------------

The functions and Macros in this section are used to control the creation,
memory allocation, and specific destruction of ``float_v`` data types.

init_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: float_v* init_float_vector(size_t buffer)

   Initializes a new dynamically allocated float vector with specified initial capacity.
   The vector will automatically grow if needed when adding elements.

   :param buffer: Initial capacity (number of floats) to allocate
   :returns: Pointer to new float_v object, or NULL on allocation failure
   :raises: Sets errno to EINVAL if buffer is 0, ENOMEM if memory allocation fails

   Example:

   .. code-block:: c

      // Create vector with initial capacity for 10 floats
      float_v* vec = init_float_vector(10);
      if (!vec) {
          fprintf(stderr, "Failed to initialize vector\\n");
          return 1;
      }
      
      // Free when done
      free_float_vector(vec);

init_float_array
~~~~~~~~~~~~~~~~
.. c:macro:: init_float_array(size)

   Creates a statically allocated float array wrapped in a float_v structure.
   Must be used at variable declaration.

   :param size: Size of the static array to create
   :returns: float_v structure containing the static array

   Example:

   .. code-block:: c

      // Create static array of 10 floats
      float_v arr = init_float_array(10);
      
      // No need to free - automatically cleaned up when out of scope

free_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: void free_float_vector(float_v* vec)

   Frees all memory associated with a dynamically allocated float vector.
   This function should only be used with vectors created by init_float_vector().
   For vectors created with init_float_array(), this function will return an error.
   If using a ``gcc`` or ``clang`` compiler, consider using the FLTVEC_GBC macro
   instead.

   :param vec: Float vector to free
   :raises: Sets errno to EINVAL if vec is NULL or if attempting to free a static array

   Example:

   .. code-block:: c

      float_v* vec = init_float_vector(5);
      
      // Use the vector...
      
      // Free the vector when done
      free_float_vector(vec);
      vec = NULL;  // Good practice to avoid dangling pointer

.. warning::

   The free_float_vector() function should only be used with dynamically allocated
   vectors created by init_float_vector(). Attempting to free a static

Automatic Cleanup
-----------------

The C language does not generally allow automated garbage collection of memory that
is out of scope. This section describes a poor man's garbage collection within the
C language, for the ``float_v`` data type, that can only be enabled if compiled
with ``gcc`` or ``clang``.

FLTVEC_GBC
~~~~~~~~~~
.. c:macro:: FLTVEC_GBC

   Macro that enables automatic cleanup of dynamically allocated float vectors when they
   go out of scope. Only available when using GCC or Clang compilers. Uses the cleanup
   attribute to automatically call _free_float_vector.

   Example:

   .. code-block:: c

      void process_floats(void) {
          // Vector will be automatically freed when function returns
          FLTVEC_GBC float_v* vec = init_float_vector(10);
          
          // Use the vector...
          
          // No need to call free_float_vector
      }  // vec is automatically freed here

   .. note::

      This macro should only be used with dynamically allocated vectors created by
      init_float_vector(). It is not needed for static arrays created with
      init_float_array() as they are automatically cleaned up when going out of scope.

[Previous content remains the same until "Automatic Cleanup" section...]

Utility Functions
=================
The following functions and macros can be used to retrieve basic information from
float containers.

Vector Size and Access Functions
--------------------------------
The following functions and macros are used to determine the allocated size and populated
length of float containers.

.. _float-size-func:

float_vector_size
~~~~~~~~~~~~~~~~~
.. c:function:: const size_t float_vector_size(const float_v* vec)

   Returns the current number of elements in the vector. This represents the
   actual number of floats stored, not the allocated capacity. 
   The :ref:`f_alloc <f-alloc-macro>` generic macro can be used in place of this 
   function. 

   :param vec: Float vector to query
   :returns: Number of elements in vector, or LONG_MAX on error
   :raises: Sets errno to EINVAL for NULL input or invalid data pointer

   Example:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(5);  // Capacity of 5
      push_back_float_vector(vec, 1);
      push_back_float_vector(vec, 2);
      push_back_float_vector(vec, 3);
      printf("Vector size: %zu\n", float_vector_size(vec));

   Result 

   .. code-block:: bash 

      Vector size: 3

.. _float-alloc-func:

float_vector_alloc
~~~~~~~~~~~~~~~~~~
.. c:function:: const size_t float_vector_alloc(const float_v* vec)

   Returns the current allocation size (capacity) of the vector. This represents
   the number of elements that can be stored without requiring reallocation.
   The :ref:`f_alloc <f-alloc-macro>` generic macro can be used in place of this 
   function.

   :param vec: Float vector to query
   :returns: Current allocation size, or LONG_MAX on error
   :raises: Sets errno to EINVAL for NULL input or invalid data pointer

   Example:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(5);
      printf("Allocation size: %zu\n", float_vector_alloc(vec));

   Result 

   .. code-block:: bash 

      Allocation size: 5
