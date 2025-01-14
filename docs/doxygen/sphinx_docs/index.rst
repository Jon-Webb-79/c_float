.. Core Utilities documentation master file, created by
   sphinx-quickstart
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to C Float Documentation!
=================================

Working with floating-point numbers in C presents several challenges in modern software development:

* Manual memory management for arrays of floats can lead to memory leaks and buffer overflows
* Static arrays require compile-time size decisions that may not suit runtime needs
* No built-in support for dynamic resizing of float collections
* Lack of associative containers for float values requires complex custom implementations
* Error handling for floating-point operations often requires boilerplate code

The c_float library addresses these challenges by providing three primary container types:

* A dynamic float vector that automatically manages memory and resizing
* A safe wrapper for static float arrays with bounds checking
* A dictionary implementation for mapping strings to float values

When to Use This Library
########################

All of the functionality in this library can be accessed from the ``c_float.h`` file.
This library is particularly useful when:

* Working with float arrays of unknown or varying size
* Requiring safe bounds-checked access to float arrays
* Managing collections of named floating-point values
* Performing numerical computations with dynamic data sets
* Implementing algorithms that require flexible float storage

The library's encapsulated design prevents common floating-point array manipulation errors while maintaining the performance characteristics expected in C programs.

This project encapsulates its functionality into the ``c_float.h`` header file, which is wrapped in the header guard ``#ifdef __cplusplus`` to allow compilation with both C and C++. The header file name ``c_float.h`` was chosen to maintain consistency with C naming conventions while avoiding potential conflicts with existing system headers.

Implementation Details
######################

The library provides three main container types:

Dynamic Float Vector
--------------------
* Automatically manages memory allocation and resizing
* Maintains size and capacity information
* Provides safe element access with bounds checking
* Supports efficient append and insert operations

Static Float Array Wrapper
--------------------------
* Provides bounds-checked access to fixed-size arrays
* Prevents buffer overflows through runtime checks
* Maintains actual element count for partially filled arrays
* Offers safe iteration and access methods

Float Dictionary
----------------
* Maps string keys to float values
* Handles memory management for both keys and values
* Provides efficient key lookup
* Supports dynamic addition and removal of entries

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   Dynamically Allocated Vectors <Vector>

Indices and tables
##################

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

Contributing
############
Pull requests are welcome.  For major changes, please open an issue first to discuss
what you would like to change.  Please make sure to include and update tests
as well as relevant doc-string and sphinx updates.

License
#######
The License is included in the **c_string** package

Requirements
############
This library is developed and tested on Macintosh and Arch Linux Operating
Systems.  It is developed with ``gcc 14.2.1`` and ``clang 16.0.6`` compilers. In
addition, this code base requires the use of ``CMake 3.31.3``, ``cmocka``, and 
``valgrind``.  This code is compiled and written with the C17 standard; however, this 
should work with any compiler using C11 or later versions.

Installation and Build Guide
############################

Requirements
------------
- Git
- CMake (version 3.31.3 or later)
- C compiler (GCC, Clang, or MSVC)

For unit testing:
- Linux: valgrind (optional, for memory leak checking)
- All platforms: cmocka testing framework

Getting the Code
----------------
Clone the repository:

.. code-block:: bash

  git clone https://github.com/Jon-Webb-79/c_float.git
  cd c_float

Debug Build (with tests)
------------------------

Use the appropriate script for your platform:

**Linux/macOS (bash)**:

.. code-block:: bash

  cd scripts/bash
  ./debug.sh

**Linux/macOS (zsh)**:

.. code-block:: bash

  cd scripts/zsh
  ./debug.zsh

**Windows**:

.. code-block:: batch

  cd scripts\Windows
  debug.bat

Run tests:

**Linux (with valgrind)**:

.. code-block:: bash

  cd build/debug
  valgrind ./unit_tests

**macOS/Windows**:

.. code-block:: bash

  cd build/debug
  ./unit_tests

Static Library Build
--------------------
Creates a static library without tests:

**Linux/macOS (bash)**:

.. code-block:: bash

  cd scripts/bash
  ./static.sh

**Linux/macOS (zsh)**:

.. code-block:: bash

  cd scripts/zsh
  ./static.zsh

**Windows**:

.. code-block:: batch

  cd scripts\Windows
  static.bat

System Installation
-------------------
Installs library files to system directories for use in other projects:

**Linux/macOS (requires sudo)**:

.. code-block:: bash

  cd scripts/bash  # or scripts/zsh
  sudo ./install.sh  # or sudo ./install.zsh

**Windows (requires Administrator)**:

1. Right-click ``scripts\Windows\install.bat``
2. Select "Run as Administrator"

Usage in Projects
-----------------
After installation, you can use the library in three ways:

1. **As System Library**:

  After installation, include in your C files:

  .. code-block:: c

     #include <c_float.h>

2. **As Static Library**:

  Link against the static library created in the build/static directory.

3. **Direct Integration**:

  Copy ``c_float.c`` and ``c_float.h`` directly into your project.

Troubleshooting
---------------
- If tests fail, ensure all dependencies are properly installed
- For Windows builds, ensure you're using an appropriate Visual Studio version
- For installation issues, verify you have appropriate system permissions

Contribute to Code Base 
-----------------------
#. Establish a pull request with the git repository owner.

#. Once the package has been downloade, you will also need to install
   Python3.10 or later version to support documentation with Sphinx.

#. Navigate to the ``c_float/docs/doxygen`` directory.

#. Create a Python virtual environment with the following command.

   .. code-block:: bash 

      python -m venv .venv 

#. Activate the virtual environment with the following command.

.. table:: Activation Commands for Virtual Environments

   +----------------------+------------------+-------------------------------------------+
   | Platform             | Shell            | Command to activate virtual environment   |
   +======================+==================+===========================================+
   | POSIX                | bash/zsh         | ``$ source <venv>/bin/activate``          |
   +                      +------------------+-------------------------------------------+
   |                      | fish             | ``$ source <venv>/bin/activate.fish``     |
   +                      +------------------+-------------------------------------------+
   |                      | csh/tcsh         | ``$ source <venv>/bin/activate.csh``      |
   +                      +------------------+-------------------------------------------+
   |                      | Powershell       | ``$ <venv>/bin/Activate.ps1``             |
   +----------------------+------------------+-------------------------------------------+
   | Windows              | cmd.exe          | ``C:\> <venv>\\Scripts\\activate.bat``    |
   +                      +------------------+-------------------------------------------+
   |                      | PowerShell       | ``PS C:\\> <venv>\\Scripts\\Activate.ps1``|
   +----------------------+------------------+-------------------------------------------+

#. Install packages to virtual environments from ``requirements.txt`` file

   .. code-block:: bash 

      pip install -r requirements.txt

#. At this point you can build the files in the same way described in the 
   previous section and contribute to documentation.
