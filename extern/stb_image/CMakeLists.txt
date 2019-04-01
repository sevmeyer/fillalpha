cmake_minimum_required(VERSION 3.11)
include(FetchContent)

# Target:  stb_image
# Include: stb_image/stb_image.h
# Version: 2.22
# License: Public Domain

# Files
FetchContent_Declare(stb_image
	URL "https://github.com/nothings/stb/raw/2c2908f50515dcd939f24be261c3ccbcd277bb49/stb_image.h"
	URL_HASH SHA256=0e28238d865510073b5740ae8eba8cd8032cc5b25f94e0f7505fac8036864909
	DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/stb_image"
	DOWNLOAD_NO_EXTRACT TRUE)

# Download
FetchContent_GetProperties(stb_image)
if(NOT stb_image_POPULATED)
	message(STATUS "Fetching external library: stb_image")
	FetchContent_Populate(stb_image)
endif()

# Target
add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")
