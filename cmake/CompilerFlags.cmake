add_definitions(-DUNICODE)
add_definitions(-DQT_MESSAGELOGCONTEXT)
add_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-DQT_NO_EXCEPTIONS)

if(NOT MSVC AND NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC" AND QT5)
	add_definitions(-DQT_STRICT_ITERATORS)
endif()

if(CMAKE_VERSION VERSION_LESS "3.12")
	add_definitions(-DQT_RESTRICTED_CAST_FROM_ASCII)
else()
	add_compile_definitions($<$<CONFIG:Debug>:QT_RESTRICTED_CAST_FROM_ASCII>)
	add_compile_definitions($<$<NOT:$<CONFIG:Debug>>:QT_NO_CAST_FROM_ASCII>)
endif()

if(QT_VENDOR STREQUAL "Governikus")
	add_definitions(-DGOVERNIKUS_QT)

	if(QT_VERSION VERSION_LESS_EQUAL "5.12")
		add_definitions(-DQT_DEPRECATED_WARNINGS)
	endif()

	add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x051500)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT DEFINED WARNINGS_ARE_ERRORS AND VENDOR_GOVERNIKUS)
	set(WARNINGS_ARE_ERRORS ON)
endif()


if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
	if(WARNINGS_ARE_ERRORS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
	endif()
	ADD_FLAG(/Qspectre)
else()
	string(REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	string(REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wcast-qual -Wshadow")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -g")

	if(ANDROID AND CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
		set(PREFER_LD bfd CACHE STRING "")
	else()
		set(PREFER_LD gold CACHE STRING "")
	endif()

	ADD_FLAG(-fuse-ld=${PREFER_LD} VAR CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS USE_SAME_FOR_LINKER USE_LINKER_ONLY)
	ADD_FLAG(-flto VAR CMAKE_EXE_LINKER_FLAGS_RELEASE CMAKE_SHARED_LINKER_FLAGS_RELEASE USE_SAME_FOR_LINKER)

	if(NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
		ADD_FLAG(-fno-rtti VAR CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
	endif()

	ADD_FLAG(-fno-exceptions)
	ADD_FLAG(-fstack-protector-strong -fstack-protector)
	ADD_FLAG(-Wold-style-cast)
	ADD_FLAG(-Wcast-align=strict -Wcast-align)
	ADD_FLAG(-Wmost)
	ADD_FLAG(-Wpedantic)
	ADD_FLAG(-Wvla)
	ADD_FLAG(-Wconversion)
	ADD_FLAG(-Wloop-analysis)
	ADD_FLAG(-Wlogical-op)
	ADD_FLAG(-Wmisleading-indentation)
	ADD_FLAG(-Wduplicated-cond)
	ADD_FLAG(-Wweak-vtables)
	ADD_FLAG(-Wcovered-switch-default)
	ADD_FLAG(-Wduplicated-branches)
	ADD_FLAG(-Wdocumentation)
	ADD_FLAG(-Wsuggest-override)
	ADD_FLAG(-Winconsistent-missing-override)
	ADD_FLAG(-Winconsistent-missing-destructor-override)
	ADD_FLAG(-Wnon-virtual-dtor)
	ADD_FLAG(-Winitializer-overrides)
	ADD_FLAG(-Wunreachable-code-aggressive)
	ADD_FLAG(-Wnewline-eof)
	ADD_FLAG(-Wdate-time)
	ADD_FLAG(-Wunused)
	ADD_FLAG(-Wunused-template)
	ADD_FLAG(-Wuseless-cast)
	ADD_FLAG(-Wextra-semi)
	ADD_FLAG(-Wextra-semi-stmt)
	ADD_FLAG(-Wempty-init-stmt)
	ADD_FLAG(-Wno-gnu-zero-variadic-macro-arguments) # Qt (qDebug) is not compatible

	if(ANDROID OR INTEGRATED_SDK)
		set(CMAKE_CXX_VISIBILITY_PRESET hidden)
	endif()

	if(ANDROID)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
		if(CMAKE_COMPILER_IS_GNUCXX)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-limit=64")
		elseif(CMAKE_VERSION VERSION_LESS "3.13" AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
			list(APPEND CMAKE_CXX_COMPILER_PREDEFINES_COMMAND "--target=${CMAKE_CXX_ANDROID_TOOLCHAIN_MACHINE}")
		endif()
	endif()


	if(WARNINGS_ARE_ERRORS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	endif()

	if(APPLE AND NOT CMAKE_SYSTEM_VERSION VERSION_LESS 14)
		# Allow warning for SMCopyAllJobDictionaries, Apple will provide an alternative, until then we are stuck with this deprecated method
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations")
	endif()

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

	if(SANITIZER)
		if(NOT SANITIZER_SKIP_ASAN)
			set(SANITIZER_FLAGS "-fsanitize=address")
			set(SANITIZER_LINK_FLAGS "-fsanitize=address")
		endif()
		set(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
		ADD_FLAG("${SANITIZER_FLAGS} -fsanitize=unsigned-integer-overflow -fsanitize-address-use-after-scope" "${SANITIZER_FLAGS}" LINK "${SANITIZER_LINK_FLAGS} -fsanitize=undefined")
	endif()

	if(CMAKE_CXX_COMPILER_ID STREQUAL Intel)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd1875,1682,2259,654,177,1599")
	endif()

	if(CMAKE_CXX_COMPILER MATCHES "clazy")
		set(clazy_level level0,level1,level2)
		set(clazy_extra_warnings isempty-vs-count,signal-with-return-value,tr-non-literal,detaching-member)
		set(clazy_disabled_warnings no-copyable-polymorphic,no-ctor-missing-parent-argument,no-fully-qualified-moc-types,no-function-args-by-value)

		set(clazy_all "${clazy_level},${clazy_extra_warnings},${clazy_disabled_warnings}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -plugin-arg-clazy -Xclang ${clazy_all}")
	endif()


	# QTBUG-82978
	if(CMAKE_CXX_FLAGS MATCHES "-Wextra-semi-stmt")
		list(APPEND INCOMPATIBLE_QT_COMPILER_FLAGS "-Wno-extra-semi-stmt")
	endif()

	if(CMAKE_CXX_FLAGS MATCHES "-Wuseless-cast")
		list(APPEND INCOMPATIBLE_QT_COMPILER_FLAGS "-Wno-useless-cast")
	endif()
endif()


include(CompilerFlagsPersoSim)


# http://gcc.gnu.org/onlinedocs/gcc/Gcov.html
# http://llvm.org/docs/CommandGuide/llvm-cov.html
option(COVERAGE "Enable code coverage")
if(COVERAGE)
	message(STATUS "Enable code coverage instrumentation")
	ADD_FLAG(--coverage VAR CMAKE_CXX_FLAGS_DEBUG USE_SAME_FOR_LINKER)
endif()
