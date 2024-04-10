# This file will be included in Tools.cmake AND after Libraries.cmake
# So this file will be called two times and the check needs to respect that
# with a "VALIDATOR function" or "if(NOT VARIABLE)".

if(NOT TARGET format.qml)
	set(QMLFORMAT_MIN_VERSION 6)
	function(qmlformat_validator validator_result binary)
		execute_process(COMMAND ${binary} --version OUTPUT_VARIABLE QMLFORMAT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
		string(REPLACE "qmlformat " "" QMLFORMAT_VERSION "${QMLFORMAT_VERSION}")

		if("${QMLFORMAT_VERSION}" VERSION_LESS "${QMLFORMAT_MIN_VERSION}")
			set(${validator_result} FALSE PARENT_SCOPE)
		endif()
	endfunction()

	if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.25")
		set(VALIDATOR VALIDATOR qmlformat_validator)
	endif()

	find_program(QMLFORMAT qmlformat HINTS "${QT_INSTALL_ARCHDATA}/bin" ${VALIDATOR} CMAKE_FIND_ROOT_PATH_BOTH)
	if(QMLFORMAT)
		execute_process(COMMAND ${QMLFORMAT} --version OUTPUT_VARIABLE QMLFORMAT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
		string(REPLACE "qmlformat " "" QMLFORMAT_VERSION "${QMLFORMAT_VERSION}")

		if("${QMLFORMAT_VERSION}" VERSION_LESS "${QMLFORMAT_MIN_VERSION}")
			unset(QMLFORMAT CACHE) # let's retry later
		else()
			file(GLOB_RECURSE FILES_QML ${PROJECT_SOURCE_DIR}/*.qml)
			set(QMLFORMAT_CMD ${QMLFORMAT} -v && ${QMLFORMAT} -i -n -l unix -t -w 4)

			set(FORMATTING_FILE ${PROJECT_BINARY_DIR}/formatting.files.qml)
			file(WRITE ${FORMATTING_FILE} "")
			foreach(file ${FILES_QML})
					file(APPEND ${FORMATTING_FILE} ${file})
					file(APPEND ${FORMATTING_FILE} "\n")
			endforeach()

			add_custom_target(format.qml COMMAND ${QMLFORMAT_CMD} -F ${FORMATTING_FILE} SOURCES ${FILES_QML})
			add_dependencies(format format.qml)
		endif()
	endif()
endif()
