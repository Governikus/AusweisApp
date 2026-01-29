cmake_minimum_required(VERSION 3.25)

###########################################
#### Usage: cmake -DCMD= -P cmake/cmd.cmake
###########################################

function(EXECUTE)
	execute_process(COMMAND ${ARGV} COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY)
endfunction(EXECUTE)

function(MESSAGE type)
	if(ARGV0 STREQUAL "STDOUT")
		execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
	else()
		_MESSAGE(${type} "${ARGN}")
	endif()
endfunction()

function(CHECK_WIX_LOG EXPECTED_LOGGING)
	list(LENGTH EXPECTED_LOGGING EXPECTED_COUNT)

	file(STRINGS "${FILE}" WIX_WARNINGS REGEX "warning|error")
	foreach(m ${WIX_WARNINGS})
		unset(KNOWN_WARNING)
		foreach(e ${EXPECTED_LOGGING})
			string(REGEX MATCH "${e}" KNOWN_WARNING "${m}")
			if(KNOWN_WARNING)
				MATH(EXPR WARNING_COUNT "${WARNING_COUNT}+1")
				break()
			endif()
		endforeach()
		if(NOT KNOWN_WARNING)
			message(STATUS "${m}\n")
			MATH(EXPR UNKNOWN_WARNINGS "${UNKNOWN_WARNINGS}+1")
		endif()
	endforeach()

	if(UNKNOWN_WARNINGS)
		message(FATAL_ERROR "WiX log: Found ${UNKNOWN_WARNINGS} new warnings")
	endif()

	if(WARNING_COUNT EQUAL EXPECTED_COUNT)
		message(STATUS "WiX log: Expectation met")
		return()
	endif()

	if(WARNING_COUNT GREATER EXPECTED_COUNT)
		message(FATAL_ERROR "WiX log: Found unexpected warnings")
	else()
		message(FATAL_ERROR "WiX log: Missing expected warnings")
	endif()
endfunction()

function(CHECK_WIX_MSI)
	list(APPEND EXPECTED_LOGGING "warning WIX1077.*WixShellExecTarget.*INSTALL_ROOT")
	list(APPEND EXPECTED_LOGGING "warning WIX1077.*WixShellExecTarget.*ProductName")

	CHECK_WIX_LOG("${EXPECTED_LOGGING}")
endfunction()

function(CHECK_WIX_VALIDATION)
	list(APPEND EXPECTED_LOGGING "warning WIX1076.*ICE03.*String overflow.*CustomInstallDirDlg\\.SystemSettingsCheckBox")
	list(APPEND EXPECTED_LOGGING "warning WIX1076.*ICE30.*AusweisApp2.*ProxyService.*mutually exclusive")
	list(APPEND EXPECTED_LOGGING "warning WIX1076.*ICE30.*AusweisApp2.*ProxyService.*mutually exclusive")
	list(APPEND EXPECTED_LOGGING "error WIX0204.*ICE38.*DesktopShortcut.*user profile.*KeyPath.*HKCU")
	list(APPEND EXPECTED_LOGGING "error WIX0204.*ICE38.*StartmenuShortcut.*user profile.*KeyPath.*HKCU")
	list(APPEND EXPECTED_LOGGING "error WIX0204.*ICE43.*DesktopShortcut.*non-advertised shortcuts.*KeyPath.*HKCU")
	list(APPEND EXPECTED_LOGGING "error WIX0204.*ICE43.*StartmenuShortcut.*non-advertised shortcuts.*KeyPath.*HKCU")
	list(APPEND EXPECTED_LOGGING "error WIX0204.*ICE57.*'DesktopShortcut' has both.*data with a per-machine KeyPath")
	list(APPEND EXPECTED_LOGGING "error WIX0204.*ICE57.*'StartmenuShortcut' has both.*data with a per-machine KeyPath")
	list(APPEND EXPECTED_LOGGING "warning WIX1076.*ICE61.*This product should remove only older versions of itself.")

	CHECK_WIX_LOG("${EXPECTED_LOGGING}")
endfunction()

function(CHECK_FAILURE_CODES)
	file(STRINGS "src/global/FailureCode.h" LINES)
	foreach(line ${LINES})
		if(line MATCHES "enum class Reason")
			set(ENUM_FOUND 1)
			continue()
		endif()

		if(ENUM_FOUND)
			if(line MATCHES "{")
				continue()
			endif()

			if(line MATCHES "}")
				break()
			endif()

			string(REGEX REPLACE "//.*" "" line ${line})
			string(APPEND SINGLE_LINE ${line})
		endif()
	endforeach()
	string(REGEX REPLACE "/\\*[^(\\*/)]*\\*/" "" SINGLE_LINE ${SINGLE_LINE})
	string(REGEX MATCHALL "([a-zA-Z0-9_]+)" FAILURE_CODES ${SINGLE_LINE})

	file(STRINGS "docs/failurecodes/failurecodes.rst" LINES_RST)
	foreach(line_rst ${LINES_RST})
		#match this pattern: "  - | **Card_Removed**" with spaces or tabs as whitespaces.
		string(REGEX MATCH "[ \\t]+-[ \\t]+\\|[ \\t]+\\*\\*([A-Za-z0-9_]+)\\*\\*" MATCH ${line_rst})
		if(MATCH)
			list(APPEND FAILURE_CODES_RST ${CMAKE_MATCH_1})
		endif()
	endforeach()

	list(LENGTH FAILURE_CODES ENUM_CODE_COUNT)
	list(LENGTH FAILURE_CODES_RST RST_CODE_COUNT)
	if(NOT ENUM_CODE_COUNT EQUAL RST_CODE_COUNT)
		message(FATAL_ERROR
			"The failure code count in FailureCode.h (${ENUM_CODE_COUNT}) does not match the count in failurecodes.rst (${RST_CODE_COUNT})!")
	endif()

	file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
	foreach(code ${FAILURE_CODES})
		if(NOT code IN_LIST FAILURE_CODES_RST)
			message(FATAL_ERROR
				"The failure code [${code}] is not part of failurecodes.rst. This hints a missing, duplicated or incorrectly spelled code in failurecodes.rst.")
		endif()

		set(COUNTER 0)
		foreach(file ${SOURCE_FILES})
			file(READ ${file} CONTENT)
			string(REGEX MATCHALL "FailureCode::Reason::${code}" USAGE ${CONTENT})
			list(LENGTH USAGE OCCURRENCES)
			math(EXPR COUNTER ${COUNTER}+${OCCURRENCES})
		endforeach()
		if(NOT COUNTER EQUAL 1)
			message(FATAL_ERROR "${code} is not used exactly one time. Found ${COUNTER}")
		endif()
	endforeach()
endfunction()

function(CHECK_QMLDIR)
	file(GLOB_RECURSE QMLDIRS "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

	if(QMLDIRS STREQUAL "")
		message(FATAL_ERROR "no qmldirs found: ${CMAKE_CURRENT_BINARY_DIR}")
	endif()

	foreach(file ${QMLDIRS})
		get_filename_component(dir "${file}" DIRECTORY)
		file(GLOB_RECURSE QMLFILES RELATIVE "${dir}" "${dir}/*.qml")
		set(flatten_qmlfiles)
		foreach(entry ${QMLFILES})
			get_filename_component(name "${entry}" NAME)
			list(APPEND flatten_qmlfiles ${name})
		endforeach()

		file(STRINGS "${file}" content)
		# check if qmldir has invalid files
		foreach(line ${content})
			string(REGEX MATCH "[A-Za-z]+\\.qml$" _unused "${line}")
			if(CMAKE_MATCH_0)
				list(FIND flatten_qmlfiles ${CMAKE_MATCH_0} found)
				if(found EQUAL -1)
					string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}/" "" subdir "${file}")
					message(STATUS "${CMAKE_MATCH_0} not found in ${subdir}")
					set(failed TRUE)
				endif()
			endif()
		endforeach()

		# check if qmldir has missing files
		foreach(qmlfile ${flatten_qmlfiles})
			string(REGEX MATCH "([A-Za-z]+)_[0-9\\.]+\\.qml" _unused "${qmlfile}")
			if(CMAKE_MATCH_1)
				set(qmlfile "${CMAKE_MATCH_1}.qml")
			endif()
			file(STRINGS "${file}" content REGEX " ${qmlfile}")
			if(content STREQUAL "")
				string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}/" "" qmldir "${file}")
				message(STATUS "${qmlfile} not found in ${qmldir}")
				set(failed TRUE)
			endif()
		endforeach()
	endforeach()

	if(failed)
		message(FATAL_ERROR "qmldir seems invalid")
	endif()
endfunction()

function(CHECK_QMLTYPES)
	file(GLOB_RECURSE QMLTYPES "${CMAKE_CURRENT_BINARY_DIR}/*.qmltypes")

	if(QMLTYPES STREQUAL "")
		message(FATAL_ERROR "no qmltypes found: ${CMAKE_CURRENT_BINARY_DIR}")
	endif()

	set(failed FALSE)
	foreach(file ${QMLTYPES})
		file(STRINGS "${file}" filecontent)
		set(PARENT_NAME "")
		set(ENUM_FOUND FALSE)
		set(ENUM_NAME "")
		set(ENUM_SCOPED FALSE)

		foreach(line ${filecontent})
			if(ENUM_FOUND)
				if(line MATCHES "}")
					if(NOT ENUM_SCOPED AND NOT PARENT_NAME MATCHES "governikus::Enum${ENUM_NAME}" AND NOT PARENT_NAME MATCHES "^Q.*")
						get_filename_component(file "${file}" NAME)
						message(STATUS "${file}: ${ENUM_NAME} of ${PARENT_NAME} is not scoped")
						set(failed TRUE)
					endif()
					set(ENUM_FOUND FALSE)
					set(ENUM_NAME "")
					set(ENUM_SCOPED FALSE)
				elseif(line MATCHES "name: \"([A-Za-z]+)\"")
					set(ENUM_NAME ${CMAKE_MATCH_1})
				elseif(line MATCHES "isScoped: true")
					set(ENUM_SCOPED TRUE)
				endif()
			elseif(line MATCHES "Enum {")
				set(ENUM_FOUND TRUE)
			elseif(line MATCHES "name: \"([A-Za-z:]+)\"")
				set(PARENT_NAME ${CMAKE_MATCH_1})
			endif()
		endforeach()
	endforeach()

	if(failed)
		message(FATAL_ERROR "Enum in qmltypes is not scoped")
	endif()
endfunction()

function(CHECK_QMLENUMS)
	file(GLOB_RECURSE QML "${CMAKE_CURRENT_BINARY_DIR}/*.qml")

	foreach(file ${QML})
		file(STRINGS "${file}" filecontent)
		unset(ENUM_NAME)

		foreach(line ${filecontent})
			if(ENUM_NAME)
				if(line MATCHES "}")
					unset(ENUM_NAME)
				elseif(line MATCHES "([A-Za-z0-9_]+)")
					list(APPEND ENUMS "${ENUM_NAME}.${CMAKE_MATCH_1}")
				endif()
			elseif(NOT ENUM_NAME AND line MATCHES "enum ([A-Za-z]+) {")
				cmake_path(GET file STEM stem)
				set(ENUM_NAME "${stem}.${CMAKE_MATCH_1}")
			endif()
		endforeach()
	endforeach()

	unset(bad_enum_regex)
	foreach(enum IN LISTS ENUMS)
		cmake_path(GET enum STEM enum_component)
		cmake_path(GET enum EXTENSION LAST_ONLY enum_value)
		list(APPEND bad_enum_regex ${enum_component}${enum_value})
	endforeach()
	list(JOIN bad_enum_regex "|" bad_enum_regex)

	foreach(file ${QML})
		file(STRINGS "${file}" filecontent REGEX "(${bad_enum_regex})")
		if(filecontent)
			message(STATUS "File contains wrong values: ${file}")
			set(failed TRUE)

			foreach(line ${filecontent})
				foreach(enum IN LISTS ENUMS)
					cmake_path(GET enum STEM enum_component)
					cmake_path(GET enum EXTENSION LAST_ONLY enum_value)
					if(line MATCHES "${enum_component}${enum_value}")
						message("   Value: ${enum_component}${enum_value}\n   Enum: ${enum}\n   Line: ${line}")
					endif()
				endforeach()
			endforeach()
		endif()
	endforeach()

	if(failed)
		message(FATAL_ERROR "Enum in QML is not scoped")
	endif()
endfunction()

function(GENERATE_APPCAST)
	set(APPCAST_URL https://updates.autentapp.de)
	set(APPCAST_ITEM "
		{
			\"date\": \"APPCAST_DATE\",
			\"platform\": \"APPCAST_PLATFORM\",
			\"minimum_platform\": \"APPCAST_MINIMUM_PLATFORM\",
			\"version\": \"APPCAST_VERSION\",
			\"url\": \"APPCAST_URL\",
			\"size\": APPCAST_SIZE,
			\"checksum\": \"APPCAST_CHECKSUM\",
			\"notes\": \"APPCAST_NOTES\"
		}")
	set(APPCAST "{
	\"items\":
	[APPCAST_ITEMS
	]
}")

	macro(ADD_APPCAST_FILE _files _system _min)
		string(TIMESTAMP APPCAST_DATE "%Y-%m-%dT%H:%M:%S")

		foreach(filePath ${_files})
			file(SIZE ${filePath} fileSize)
			get_filename_component(file ${filePath} NAME)

			if(NOT DEFINED fileSize)
				message(FATAL_ERROR "Cannot get file size of: ${file}")
			endif()

			message(STATUS "Processing: ${file}")

			string(REPLACE "AusweisApp-" "" APPCAST_FILE_VERSION ${file})
			string(REPLACE ".msi" "" APPCAST_FILE_VERSION ${APPCAST_FILE_VERSION})

			set(item ${APPCAST_ITEM})
			string(REPLACE "APPCAST_DATE" "${APPCAST_DATE}" item ${item})
			string(REPLACE "APPCAST_PLATFORM" ${_system} item ${item})
			string(REPLACE "APPCAST_MINIMUM_PLATFORM" ${_min} item ${item})
			string(REPLACE "APPCAST_VERSION" "${APPCAST_FILE_VERSION}" item ${item})
			string(REPLACE "APPCAST_URL" "${APPCAST_URL}/${file}" item ${item})
			string(REPLACE "APPCAST_SIZE" "${fileSize}" item ${item})
			string(REPLACE "APPCAST_CHECKSUM" "${APPCAST_URL}/${file}.sha256" item ${item})
			string(REPLACE "APPCAST_NOTES" "${APPCAST_URL}/ReleaseNotes.html" item ${item})

			set(APPCAST_ITEMS "${APPCAST_ITEMS}${item},")
		endforeach()
	endmacro()

	file(GLOB MSI_FILES ${CMAKE_BINARY_DIR}/*.msi)
	if(MSI_FILES)
		ADD_APPCAST_FILE("${MSI_FILES}" "win" "10")
	endif()

	if(APPCAST_ITEMS)
		string(REGEX REPLACE ",$" "" APPCAST_ITEMS "${APPCAST_ITEMS}")
		string(REPLACE "APPCAST_ITEMS" "${APPCAST_ITEMS}" APPCAST "${APPCAST}")
		file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/AppcastInfo.json" CONTENT "${APPCAST}" @ONLY NEWLINE_STYLE UNIX)
	endif()
endfunction()


if(NOT CMD)
	message(FATAL_ERROR "You need to specify 'CMD'")
endif()

if(COMMAND "${CMD}")
	cmake_language(CALL ${CMD})
else()
	message(FATAL_ERROR "Unknown CMD: ${CMD}")
endif()
