
add_executable (pngrim
ImagePNG.cpp
ImagePNG.h
main.cpp
Matrix.h
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

install(TARGETS pngrim DESTINATION bin)
target_link_libraries(pngrim png)

