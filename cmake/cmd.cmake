cmake_minimum_required(VERSION 3.13.0)
cmake_policy(SET CMP0057 NEW)

###########################################
#### Usage: cmake -DCMD= -P cmake/cmd.cmake
###########################################

function(EXECUTE)
	execute_process(COMMAND ${ARGV} RESULT_VARIABLE _result)
	if(NOT ${_result} EQUAL 0)
		message(FATAL_ERROR "Process failed: ${_result}")
	endif()
endfunction(EXECUTE)

function(MESSAGE type)
	if(ARGV0 STREQUAL "STDOUT")
		execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${ARGN}")
	else()
		_MESSAGE(${type} "${ARGN}")
	endif()
endfunction()


function(CREATE_HASH)
	if(NOT FILES)
		message(FATAL_ERROR "You need to specify 'FILES'")
	endif()

	if(NOT ALGORITHM)
		set(ALGORITHM SHA256)
	endif()
	string(TOLOWER "${ALGORITHM}" HASHFILE_ENDING)

	file(GLOB GLOBBED_FILES RELATIVE "${CMAKE_CURRENT_BINARY_DIR}" "${FILES}")

	foreach(f ${GLOBBED_FILES})
		file(${ALGORITHM} ${f} fHash)
		set(OUTPUT "${fHash}  ${f}")
		message(STDOUT ${OUTPUT})
		if(CREATE_FILE)
			file(WRITE ${f}.${HASHFILE_ENDING} "${OUTPUT}\n")
		endif()
	endforeach()
endfunction()

function(CHECK_WIX_WARNING)
	list(APPEND EXPECTED_WARNINGS "CNDL1077.*WixShellExecTarget.*INSTALL_ROOT")
	list(APPEND EXPECTED_WARNINGS "CNDL1077.*WixShellExecTarget.*ProductName")
	list(APPEND EXPECTED_WARNINGS "LGHT1076.*ICE03.*CustomInstallDirDlg.SystemSettingsCheckBox")
	list(APPEND EXPECTED_WARNINGS "LGHT1076.*ICE30.*AusweisApp2.*ProxyService")
	list(APPEND EXPECTED_WARNINGS "LGHT1076.*ICE30.*AusweisApp2.*ProxyService")
	list(APPEND EXPECTED_WARNINGS "LGHT1076.*ICE61.*product.*version")
	list(LENGTH EXPECTED_WARNINGS EXPECTED_COUNT)

	file(STRINGS "${FILE}" WIX_WARNINGS REGEX "warning")
	foreach(m ${WIX_WARNINGS})
		unset(KNOWN_WARNING)
		foreach(e ${EXPECTED_WARNINGS})
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


function(IMPORT_PATCH)
	message(STATUS "Import patch...")
	find_package(Python REQUIRED)
	EXECUTE(${Python_EXECUTABLE} "${CMAKE_BINARY_DIR}/resources/jenkins/import.py")
endfunction()


function(DEPLOY_NEXUS)
	if(NOT DEFINED ENV{NEXUS_USERNAME} OR NOT DEFINED ENV{NEXUS_PSW})
		message(FATAL_ERROR "Please provide environment variable NEXUS_USERNAME and NEXUS_PSW")
	endif()

	set(GNUPG_KEY 699BF3055B0A49224EFDE7C72D7479A531451088)
	if(DEFINED ENV${GNUPG_KEY})
		set(GNUPG_KEY ENV${GNUPG_KEY})
	endif()

	find_program(MVN_BIN mvn)
	if(NOT MVN_BIN)
		message(FATAL_ERROR "Cannot find mvn")
	endif()

	set(SETTINGS_XML "<settings><servers><server>
					<id>nexus</id>
					<username>\${env.NEXUS_USERNAME}</username>
					<password>\${env.NEXUS_PSW}</password>
				</server></servers></settings>")
	file(WRITE settings.xml "${SETTINGS_XML}")

	function(get_file _suffix _out_var)
		file(GLOB file RELATIVE ${CMAKE_BINARY_DIR} ${_suffix})

		list(LENGTH file list_length)
		if(list_length GREATER 1)
			message(FATAL_ERROR "Found more than one entry: ${file}")
		elseif(asc_length EQUAL 0)
			message(FATAL_ERROR "File ${file} not found. Maybe signature is missing?: gpg -a -u ${GNUPG_KEY} --detach-sig")
		endif()

		set(${_out_var} ${file} PARENT_SCOPE)
	endfunction()

	get_file("*.aar" FILE_AAR)
	get_file("*.pom" FILE_POM)
	get_file("*-sources.jar" FILE_JAR)

	file(STRINGS "${FILE_POM}" is_snapshot REGEX "<version>.+-SNAPSHOT</version>")
	if(is_snapshot)
		set(NEXUS_URL https://repo.govkg.de/repository/ausweisapp-snapshots)
	else()
		if(PUBLIC)
			set(NEXUS_URL https://s01.oss.sonatype.org/service/local/staging/deploy/maven2)
		else()
			set(NEXUS_URL https://repo.govkg.de/repository/ausweisapp-releases)
		endif()
	endif()

	EXECUTE(${MVN_BIN} deploy:deploy-file -Dfile=${FILE_AAR} -DpomFile=${FILE_POM} -Dsources=${FILE_JAR} -DrepositoryId=nexus -Durl=${NEXUS_URL} --settings settings.xml)

	if(PUBLIC)
		get_file("*.aar.asc" FILE_AAR_ASC)
		get_file("*.pom.asc" FILE_POM_ASC)
		get_file("*-sources.jar.asc" FILE_SOURCES_ASC)

		function(mvn_upload _file _packaging _classifier)
			EXECUTE(${MVN_BIN} deploy:deploy-file -Dfile=${_file} -Dpackaging=${_packaging} -Dclassifier=${_classifier} -DpomFile=${FILE_POM} -DrepositoryId=nexus -Durl=${NEXUS_URL} --settings settings.xml)
		endfunction()

		mvn_upload("${FILE_AAR_ASC}" "aar.asc" "")
		mvn_upload("${FILE_POM_ASC}" "pom.asc" "")
		mvn_upload("${FILE_SOURCES_ASC}" "jar.asc" "sources")
	endif()

	file(REMOVE settings.xml)
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

if(NOT CMD)
	message(FATAL_ERROR "You need to specify 'CMD'")
endif()

if(CMD STREQUAL "HASH")
	CREATE_HASH()
elseif(CMD STREQUAL "CHECK_WIX_WARNING")
	CHECK_WIX_WARNING()
elseif(CMD STREQUAL "IMPORT_PATCH")
	IMPORT_PATCH()
elseif(CMD STREQUAL "DEPLOY_NEXUS")
	DEPLOY_NEXUS()
elseif(CMD STREQUAL "CHECK_FAILURE_CODES")
	CHECK_FAILURE_CODES()
else()
	message(FATAL_ERROR "Unknown CMD: ${CMD}")
endif()
