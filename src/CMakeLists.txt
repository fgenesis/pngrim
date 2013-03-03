
include_directories(
"${CMAKE_CURRENT_SOURCE_DIR}"
"${CMAKE_CURRENT_SOURCE_DIR}/zlib"
"${CMAKE_CURRENT_SOURCE_DIR}/libpng"
)

add_subdirectory (zlib)
add_subdirectory (libpng)
add_subdirectory (pngrim)
