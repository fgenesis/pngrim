set(png_SRC
png.c
png.h
pngconf.h
pngdebug.h
pngerror.c
pngget.c
pnginfo.h
pnglibconf.h
pngmem.c
pngpread.c
pngpriv.h
pngread.c
pngrio.c
pngrtran.c
pngrutil.c
pngset.c
pngstruct.h
pngtest.c
pngtrans.c
pngwio.c
pngwrite.c
pngwtran.c
pngwutil.c
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

add_library(png ${png_SRC})
target_link_libraries(png zlib)
