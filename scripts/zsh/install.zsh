#!/usr/bin/zsh
# ================================================================================
# ================================================================================
# - File:    install.zsh
# - Purpose: Install c_string library files to system directories
#
# Source Metadata
# - Author:  Jonathan A. Webb
# - Date:    January 11, 2025
# - Version: 1.0
# - Copyright: Copyright 2025, Jon Webb Inc.
# ================================================================================
# ================================================================================

# Check if running with sudo/root permissions
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (use sudo)"
    exit 1
fi

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS paths
    INCLUDE_DIR="/usr/local/include"
    LIB_DIR="/usr/local/lib"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux paths
    INCLUDE_DIR="/usr/include"
    LIB_DIR="/usr/lib"
else
    echo "Unsupported operating system"
    exit 1
fi

# Create directories if they don't exist
mkdir -p "$INCLUDE_DIR"
mkdir -p "$LIB_DIR"

# Copy header file
cp ../../c_float/c_float.h "$INCLUDE_DIR"
if [ $? -eq 0 ]; then
    echo "Header file installed successfully"
else
    echo "Error installing header file"
    exit 1
fi

# Copy source file
cp ../../c_float/c_float.c "$LIB_DIR"
if [ $? -eq 0 ]; then
    echo "Source file installed successfully"
else
    echo "Error installing source file"
    exit 1
fi

# Set permissions
chmod 644 "$INCLUDE_DIR/c_float.h"
chmod 644 "$LIB_DIR/c_float.c"

echo "Installation completed successfully"
# ================================================================================
# ================================================================================
# eof
