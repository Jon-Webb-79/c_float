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

Data Addition 
-------------
The functions below can be used to add data to a dynamically allocated array 
or a statically allocated vector.

push_back_float_vector
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_back_float_vector(float_v* vec, const float value)

   Adds a float value to the end of the vector. If needed, the vector automatically
   resizes to accommodate the new value. For vectors smaller than VEC_THRESHOLD,
   capacity doubles when full. For larger vectors, a fixed amount is added.
   This is the most efficient method for adding data to a float vector with
   a time efficiency of :math:`O(1)`. If the structure passed is for a statically allocated 
   array, the function will return ``false``, if the user tries to enter data to 
   an out of bounds index and will set ``errno`` to ``EINVAL``

   :param vec: Target float vector
   :param value: Float value to add to vector
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure

   Vector Example:

   .. code-block:: c

      float_v* vec = init_float_vector(2);
      
      // Add some values
      push_back_float_vector(vec, 3.14f);
      push_back_float_vector(vec, 2.718f);
      
      // Vector will automatically resize
      push_back_float_vector(vec, 1.414f);
      
      printf("Vector size: %zu\n", f_size(vec));
      printf("[ ");
      for (size_t i = 0; i < f_size(vec) - 1; i++) 
          printf("%f, ", float_vector_index(vec, 1));
      printf("%f ]\n", float_vector_index(vec, f_size(vec) - 1))
      
      free_float_vector(vec);

   Output::

      Vector size: 3
      [ 3.140000, 2.718000, 1.414000 ]

   Array Example:

   .. code-block:: c

      float_v* vec = init_float_array(2);
      
      // Add some values
      push_back_float_vector(vec, 3.14f);
      push_back_float_vector(vec, 2.718f);
      
      // Array will refuse third element because it is out of bounds 
      
      if (!push_back_float_vector(vec, 1.414f))
          printf("push back failed\n");
      
      printf("Array size: %zu\n", f_size(vec));
      printf("[ ");
      for (size_t i = 0; i < f_size(vec) - 1; i++) 
          printf("%f, ", float_vector_index(vec, 1));
      printf("%f ]\n", float_vector_index(vec, f_size(vec) - 1))
      
      free_float_vector(vec);

   Output::

      push back failed
      Array size: 2
      [ 3.140000, 2.718000 ]

   The following should be considered when using this function:

   * The vector must be properly initialized using init_float_vector() or init_float_array()
   * For static arrays (created with init_float_array), attempts to exceed capacity will fail
   * If reallocation fails for dynamic vectors, the original vector remains unchanged
   * Any float value can be stored, including zero, infinities, and NaN
   * The operation requires enough contiguous memory for the entire resized array in dynamic case

   .. note::

      When resizing is needed, the vector grows either by doubling (when size < VEC_THRESHOLD)
      or by adding a fixed amount (when size >= VEC_THRESHOLD). This provides efficient
      amortized performance for both small and large vectors.

push_front_float_vector
~~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: bool push_front_float_vector(float_v* vec, const float value)

   Adds a float value to the beginning of the vector, shifting all existing elements
   to the right. Automatically resizes the vector if needed when using dynamic allocation.
   This is the least efficient method for adding data to a float vector with
   a time efficiency of :math:`O(n)`.

   :param vec: Target float vector
   :param value: Float value to add at front
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or if static array is full,
           ENOMEM on allocation failure, ERANGE on size_t overflow

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add some values from the back
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      // Add value at the front
      push_front_float_vector(vec, 1.0f);
      
      // Print all values
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Vector will grow automatically if needed
      push_front_float_vector(vec, 0.0f);
      
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
   Output::

      1.0 2.0 3.0
      0.0 1.0 2.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values to static array
      push_front_float_vector(&arr, 3.0f);
      printf("After first:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      push_front_float_vector(&arr, 2.0f);
      printf("After second: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      push_front_float_vector(&arr, 1.0f);
      printf("After third:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Array is now full - this will fail
      if (!push_front_float_vector(&arr, 0.0f)) {
          printf("Cannot add to full static array\n");
      }

   Output::

      After first:  3.0
      After second: 2.0 3.0
      After third:  1.0 2.0 3.0
      Cannot add to full static array

   The following should be considered when using this function:

   * For static arrays (created with init_float_array):
     - Attempts to exceed capacity will fail with errno set to EINVAL
     - No automatic resizing occurs
   
   * For dynamic vectors (created with init_float_vector):
     - Vector will automatically resize when full
     - Growth follows the doubling strategy for small vectors
     - Growth adds fixed amount for vectors larger than VEC_THRESHOLD
   
   * Performance considerations:
     - All existing elements must be moved right by one position
     - More expensive than push_back_float_vector for large vectors
     - Memory reallocation may occur for dynamic vectors

   .. note::

      When resizing is needed in dynamic vectors, the vector grows either by
      doubling (when size < VEC_THRESHOLD) or by adding a fixed amount
      (when size >= VEC_THRESHOLD). This provides efficient amortized
      performance while preventing excessive memory usage in large vectors.

insert_float_vector
~~~~~~~~~~~~~~~~~~~
.. c:function:: bool insert_float_vector(float_v* vec, float value, size_t index)

   Inserts a float value at any valid position in the vector, shifting subsequent
   elements to the right. Automatically resizes the vector if needed when using dynamic
   allocation. The time complexity of this function varies from :math:`O(1)` 
   to :math:`O(n)` depending on the insertion position.

   :param vec: Target float vector
   :param value: Float value to insert
   :param index: Position at which to insert (0 to vec->len)
   :returns: true if successful, false on error
   :raises: Sets errno to EINVAL for NULL inputs or if static array is full,
           ERANGE for invalid index or on size_t overflow,
           ENOMEM on allocation failure

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Create initial vector
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 3.0f);
      
      // Initial state
      printf("Initial:     ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Insert 2.0 between them
      insert_float_vector(vec, 2.0f, 1);
      
      printf("After insert: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
   Output::

      Initial:     1.0 3.0
      After insert: 1.0 2.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Insert values at different positions
      insert_float_vector(&arr, 3.0f, 0);  // First insertion
      printf("First insert:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      insert_float_vector(&arr, 1.0f, 0);  // At beginning
      printf("Second insert: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      insert_float_vector(&arr, 2.0f, 1);  // In middle
      printf("Third insert:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Array is now full - this will fail
      if (!insert_float_vector(&arr, 4.0f, 1)) {
          printf("Cannot insert into full static array\n");
      }

   Output::

      First insert:  3.0
      Second insert: 1.0 3.0
      Third insert:  1.0 2.0 3.0
      Cannot insert into full static array

   The following should be considered when using this function:

   * For static arrays (created with init_float_array):
     - Attempts to exceed capacity will fail with errno set to EINVAL
     - No automatic resizing occurs
     - Must be careful not to exceed fixed size
   
   * For dynamic vectors (created with init_float_vector):
     - Vector will automatically resize when full
     - Growth follows the doubling strategy for small vectors
     - Growth adds fixed amount for vectors larger than VEC_THRESHOLD
   
   * Performance considerations:
     - Inserting at the beginning requires moving all elements (most expensive)
     - Inserting at the end is equivalent to push_back (least expensive)
     - Cost increases with number of elements that must be shifted
     - Memory reallocation may occur for dynamic vectors

   .. note::

      The valid range for index is [0, length]. An index equal to the length
      performs an append operation. Any index greater than the length will
      result in ERANGE error.

Data Removal
------------
The following functions can be used to remove ``float`` data from a dynamically 
allocated vector or statically allocated array.

pop_back_float_vector
~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float pop_back_float_vector(float_v* vec)

   Removes and returns the last element from the vector or array. This is the most
   efficient removal operation as it requires no element shifting. The time 
   complexity of this function is :math:`O(1)`.

   :param vec: Target float vector
   :returns: The removed float value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add some values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float popped = pop_back_float_vector(vec);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining size: %zu\n", f_size(vec));
      }

   Output::

      Initial values: 1.0 2.0 3.0
      Popped value: 3.0
      Remaining size: 2

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(2);
      
      // Add values to static array
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop values
      float first_pop = pop_back_float_vector(&arr);
      printf("First pop: %.1f\n", first_pop);
      
      float second_pop = pop_back_float_vector(&arr);
      printf("Second pop: %.1f\n", second_pop);
      
      // Try to pop from empty array
      float result = pop_back_float_vector(&arr);
      if (errno == ENODATA) {
          printf("Cannot pop from empty array\n");
      }

   Output::

      Initial values: 1.0 2.0
      First pop: 2.0
      Second pop: 1.0
      Cannot pop from empty array

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

   Example with error checking:

   .. code-block:: c

      float_v* vec = init_float_vector(2);
      push_back_float_vector(vec, FLT_MAX);  // Store actual FLT_MAX
      
      errno = 0;
      float value = pop_back_float_vector(vec);
      if (errno == 0) {
          // This was a valid FLT_MAX stored in the vector
          printf("Valid FLT_MAX popped\n");
      } else if (errno == ENODATA) {
          printf("Vector is empty\n");
      } else if (errno == EINVAL) {
          printf("Invalid vector\n");
      }
      
      free_float_vector(vec);

   Output::

      Valid FLT_MAX popped

pop_front_float_vector
~~~~~~~~~~~~~~~~~~~~~~
.. c:function:: float pop_front_float_vector(float_v* vec)

   Removes and returns the first element from the vector or array, shifting all
   remaining elements left by one position. This operation requires moving all
   remaining elements and is therefore less efficient than pop_back_float_vector.
   This function has a time complexity of :math:`O(n)`.

   :param vec: Target float vector
   :returns: The removed float value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(3);
      
      // Add some values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      float popped = pop_front_float_vector(vec);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining values: ");
          for (size_t i = 0; i < f_size(vec); i++) {
              printf("%.1f ", float_vector_index(vec, i));
          }
          printf("\n");
      }
      
   Output::

      Initial values: 1.0 2.0 3.0
      Popped value: 1.0
      Remaining values: 2.0 3.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(2);
      
      // Add values to static array
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop first value
      float first_pop = pop_front_float_vector(&arr);
      printf("First pop: %.1f\n", first_pop);
      printf("After first pop: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop remaining value
      float second_pop = pop_front_float_vector(&arr);
      printf("Second pop: %.1f\n", second_pop);
      
      // Try to pop from empty array
      float result = pop_front_float_vector(&arr);
      if (errno == ENODATA) {
          printf("Cannot pop from empty array\n");
      }

   Output::

      Initial values: 1.0 2.0
      First pop: 1.0
      After first pop: 2.0
      Second pop: 2.0
      Cannot pop from empty array

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

pop_any_float_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: float pop_any_float_vector(float_v* vec, size_t index)

   Removes and returns the element at the specified index in the vector or array,
   shifting any subsequent elements to the left. Performance varies based on the
   removal position - removing from the end is fast, while removing from the start
   or middle requires shifting elements.  This function has a time complexity that 
   range from :math:`O(n)` to :math:`O(1)` depending on the index from which 
   data is popped.

   :param vec: Target float vector
   :param index: Position of element to remove (0 to len-1)
   :returns: The removed float value, or FLT_MAX on error
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty,
           ERANGE for invalid index or on size_t overflow

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec FLTVEC_GBC = init_float_vector(4);
      
      // Add values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Pop middle value (index 1)
      float popped = pop_any_float_vector(vec, 1);
      if (errno == 0) {
          printf("Popped value: %.1f\n", popped);
          printf("Remaining values: ");
          for (size_t i = 0; i < f_size(vec); i++) {
              printf("%.1f ", float_vector_index(vec, i));
          }
          printf("\n");
      }

   Output::

      Initial values: 1.0 2.0 3.0 4.0
      Popped value: 2.0
      Remaining values: 1.0 3.0 4.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      push_back_float_vector(&arr, 3.0f);
      
      printf("Initial values: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop first value (index 0)
      float first = pop_any_float_vector(&arr, 0);
      printf("After pop first: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      // Pop last value (index 1)
      float last = pop_any_float_vector(&arr, 1);
      printf("After pop last: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Initial values: 1.0 2.0 3.0
      After pop first: 2.0 3.0
      After pop last: 2.0

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Returns FLT_MAX
     - Sets errno to EINVAL
   
   * If vector or array is empty:
     - Returns FLT_MAX
     - Sets errno to ENODATA
   
   * If index is out of bounds:
     - Returns FLT_MAX
     - Sets errno to ERANGE

   .. note::

      When FLT_MAX is returned, always check errno to distinguish between an error
      condition and a valid FLT_MAX value that was stored in the vector. If errno
      is 0, the returned FLT_MAX was a valid stored value.

   Performance Considerations:

   * Removing from the last position (index == len-1) is O(1)
   * Removing from the beginning requires shifting all elements left: O(n)
   * Removing from position i requires shifting n-i elements: O(n-i)
   * For frequent removals from the front, consider using pop_front_float_vector()
   * For frequent removals from the back, consider using pop_back_float_vector()

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

Re-Order and Sort Data 
----------------------
These functions can be used to change the order of data in a dynamically allocated 
or statically allocated ``float`` array.

reverse_float_vector
~~~~~~~~~~~~~~~~~~~~
.. c:function:: void reverse_float_vector(float_v* vec)

   Reverses the order of elements in a float vector or array. The operation is performed
   in place without allocating additional memory.

   :param vec: Target float vector
   :raises: Sets errno to EINVAL for NULL input, ENODATA if vector is empty

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec = init_float_vector(4);
      
      // Add some values
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      push_back_float_vector(vec, 3.0f);
      
      printf("Before reverse: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      reverse_float_vector(vec);
      
      printf("After reverse:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      free_float_vector(vec);

   Output::

      Before reverse: 1.0 2.0 3.0
      After reverse:  3.0 2.0 1.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(3);
      
      // Add values
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 2.0f);
      push_back_float_vector(&arr, 3.0f);
      
      printf("Before reverse: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      reverse_float_vector(&arr);
      
      printf("After reverse:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before reverse: 1.0 2.0 3.0
      After reverse:  3.0 2.0 1.0

   Error Handling:

   * If vec is NULL or has invalid data pointer:
     - Sets errno to EINVAL
     - Returns without modifying data
   
   * If vector or array is empty:
     - Sets errno to ENODATA
     - Returns without modifying data

   Performance Characteristics:

   * Time complexity: O(n) where n is the number of elements
   * Space complexity: O(1) as reversal is performed in place
   * Uses constant extra space regardless of vector size
   
   .. note::

      The function performs the reversal in place by swapping pairs of elements
      from the ends toward the middle. This approach minimizes memory usage and
      maintains efficiency for both small and large vectors.

sort_float_vector
~~~~~~~~~~~~~~~~~
.. c:function:: void sort_float_vector(float_v* vec, iter_dir direction)

   Sorts a float vector or array in either ascending (FORWARD) or descending (REVERSE) order
   using an optimized QuickSort algorithm with median-of-three pivot selection and
   insertion sort for small subarrays.

   :param vec: Target float vector
   :param direction: FORWARD for ascending, REVERSE for descending order
   :raises: Sets errno to EINVAL if vec is NULL

   Example with dynamic vector:

   .. code-block:: c

      float_v* vec = init_float_vector(5);
      
      // Add some unsorted values
      push_back_float_vector(vec, 5.0f);
      push_back_float_vector(vec, 3.0f);
      push_back_float_vector(vec, 4.0f);
      push_back_float_vector(vec, 1.0f);
      push_back_float_vector(vec, 2.0f);
      
      printf("Before sort: ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Sort in ascending order
      sort_float_vector(vec, FORWARD);
      
      printf("Ascending:   ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      // Sort in descending order
      sort_float_vector(vec, REVERSE);
      
      printf("Descending:  ");
      for (size_t i = 0; i < f_size(vec); i++) {
          printf("%.1f ", float_vector_index(vec, i));
      }
      printf("\n");
      
      free_float_vector(vec);

   Output::

      Before sort: 5.0 3.0 4.0 1.0 2.0
      Ascending:   1.0 2.0 3.0 4.0 5.0
      Descending:  5.0 4.0 3.0 2.0 1.0

   Example with static array:

   .. code-block:: c

      float_v arr = init_float_array(4);
      
      // Add unsorted values
      push_back_float_vector(&arr, 4.0f);
      push_back_float_vector(&arr, 1.0f);
      push_back_float_vector(&arr, 3.0f);
      push_back_float_vector(&arr, 2.0f);
      
      printf("Before sort: ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");
      
      sort_float_vector(&arr, FORWARD);
      
      printf("After sort:  ");
      for (size_t i = 0; i < f_size(&arr); i++) {
          printf("%.1f ", float_vector_index(&arr, i));
      }
      printf("\n");

   Output::

      Before sort: 4.0 1.0 3.0 2.0
      After sort:  1.0 2.0 3.0 4.0

   Implementation Details:

   The sorting algorithm uses a hybrid approach combining QuickSort with
   Insertion Sort for optimal performance:

   * QuickSort with median-of-three pivot selection for large partitions
   * Insertion Sort for small partitions (less than 10 elements)
   * Tail-call optimization to reduce stack usage
   * Special handling for duplicate elements and special values (NaN, infinities)

   Performance Characteristics:

   * Average time complexity: O(n log n)
   * Worst case time complexity: O(n²) (rare due to median-of-three)
   * Space complexity: O(log n) for recursion stack
   * In-place sorting: No additional memory allocation
   * Stable: No, equal elements may be reordered
   * Adaptive: Yes, performs better on partially sorted arrays

   Special Value Handling:

   * NaN values are moved to the end of the array
   * Infinities are properly ordered (-∞ < finite numbers < +∞)
   * Zero values (both -0.0 and +0.0) are treated as equal

   .. note::

      For very small arrays (n < 10), the function automatically uses Insertion Sort
      instead of QuickSort, as this is more efficient for small datasets.
