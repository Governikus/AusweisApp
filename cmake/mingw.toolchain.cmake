# This toolchain is not officially supported!
# It's just a helper to cross-compile the libraries and AusweisApp2 on Linux for Windows.

# You need to adjust your PATH variable to let AusweisApp2.exe find all necessary DLLs.
# Another approach is to copy all DLLs.
# $ cp /usr/i686-w64-mingw32/bin/*.dll .
# $ cp ~/libs/dist/bin/*.dll .
# $ cp -r ~/libs/dist/plugins/* .


set(CMAKE_SYSTEM_NAME Windows)

list(APPEND COMPILER_PREFIX "i686-w64-mingw32")
list(APPEND COMPILER_PREFIX "x86_64-w64-mingw32")

foreach(prefix ${COMPILER_PREFIX})
	find_program(CMAKE_RC_COMPILER NAMES ${prefix}-windres)
	find_program(CMAKE_C_COMPILER NAMES ${prefix}-gcc)
	find_program(CMAKE_CXX_COMPILER NAMES ${prefix}-g++)

	if(CMAKE_RC_COMPILER AND CMAKE_C_COMPILER AND CMAKE_CXX_COMPILER)
		set(CMAKE_FIND_ROOT_PATH /usr/${prefix} ${CMAKE_PREFIX_PATH})
		break()
	endif()
endforeach()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
