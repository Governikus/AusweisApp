ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-DQT_MESSAGELOGCONTEXT)
ADD_DEFINITIONS(-DQT_NO_CAST_FROM_BYTEARRAY)
ADD_DEFINITIONS(-DQT_NO_CAST_TO_ASCII)
ADD_DEFINITIONS(-DQT_RESTRICTED_CAST_FROM_ASCII)
ADD_DEFINITIONS(-DQT_NO_FOREACH)
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
ADD_DEFINITIONS(-DQT_NO_EXCEPTIONS)

IF(QT_VENDOR STREQUAL "Governikus")
	ADD_DEFINITIONS(-DGOVERNIKUS_QT)
	ADD_DEFINITIONS(-DQT_DEPRECATED_WARNINGS)
ENDIF()

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

IF(NOT DEFINED WARNINGS_ARE_ERRORS AND VENDOR_GOVERNIKUS)
	SET(WARNINGS_ARE_ERRORS ON)
ENDIF()


IF(MSVC)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
	IF(WARNINGS_ARE_ERRORS)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
	ENDIF()
	ADD_FLAG(/Qspectre)
ELSE()
	ADD_DEFINITIONS(-DQT_STRICT_ITERATORS)

	STRING(REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wcast-qual -Wshadow")

	ADD_FLAG(-flto VAR CMAKE_EXE_LINKER_FLAGS_RELEASE CMAKE_SHARED_LINKER_FLAGS_RELEASE LINK -flto)

	ADD_FLAG(-fno-exceptions)
	ADD_FLAG(-fstack-protector-strong -fstack-protector)
	ADD_FLAG(-fuse-ld=gold VAR CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS LINK -fuse-ld=gold)
	ADD_FLAG(-Wold-style-cast)
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
	ADD_FLAG(-Wno-gnu-zero-variadic-macro-arguments) # Qt (qDebug) is not compatible


	IF(ANDROID)
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
		IF(CMAKE_COMPILER_IS_GNUCXX)
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-limit=64")
		ENDIF()
		SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
	ENDIF()


	IF(WARNINGS_ARE_ERRORS)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	ENDIF()

	IF(CMAKE_COMPILER_IS_GNUCXX)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=shadow")
	ENDIF()

	IF(APPLE AND NOT CMAKE_SYSTEM_VERSION VERSION_LESS 14)
		# Allow warning for LSSharedFileListItemResolve since we support OSX 10.9, too!
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations")
	ENDIF()

	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

	IF(SANITIZER)
		IF(NOT SANITIZER_SKIP_ASAN)
			SET(SANITIZER_FLAGS "-fsanitize=address")
			SET(SANITIZER_LINK_FLAGS "-fsanitize=address")
		ENDIF()
		SET(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
		ADD_FLAG("${SANITIZER_FLAGS} -fsanitize=unsigned-integer-overflow -fsanitize-address-use-after-scope" "${SANITIZER_FLAGS}" LINK "${SANITIZER_LINK_FLAGS} -fsanitize=undefined")
	ENDIF()

	IF(CMAKE_CXX_COMPILER_ID STREQUAL Intel)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd1875,1682,2259,654,177")
	ENDIF()
ENDIF()

IF(APPLE AND NOT IOS)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ObjC++ -mmacosx-version-min=10.9")
ENDIF()


INCLUDE(CompilerFlagsPersoSim)


# http://gcc.gnu.org/onlinedocs/gcc/Gcov.html
# http://llvm.org/docs/CommandGuide/llvm-cov.html
OPTION(COVERAGE "Enable code coverage")
IF(COVERAGE)
	MESSAGE(STATUS "Enable code coverage instrumentation")
	ADD_FLAG(--coverage LINK --coverage VAR CMAKE_CXX_FLAGS_DEBUG)
ENDIF()
