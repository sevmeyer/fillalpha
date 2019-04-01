cmake_minimum_required(VERSION 3.11)

project(fillalpha)

# External libraries
add_subdirectory("extern/minarg")
add_subdirectory("extern/stb_image")
add_subdirectory("extern/stb_image_write")

# Executable
add_executable(fillalpha "src/main.cpp" "src/Image.cpp")
target_link_libraries(fillalpha PRIVATE minarg stb_image stb_image_write)

# Install
install(TARGETS fillalpha DESTINATION "bin")

# Language properties
set_property(TARGET fillalpha PROPERTY CXX_STANDARD 11)
set_property(TARGET fillalpha PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET fillalpha PROPERTY CXX_EXTENSIONS FALSE)

# Compiler options
if(MSVC)
	target_compile_options(fillalpha PRIVATE /W4 /WX)
else()
	target_compile_options(fillalpha PRIVATE -Wall -Wextra -Werror -pedantic)
	target_compile_options(fillalpha PRIVATE "$<$<CONFIG:RELEASE>:-O2>")
endif()
