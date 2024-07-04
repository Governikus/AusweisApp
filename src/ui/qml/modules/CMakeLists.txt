function(GET_BASENAME _file _output)
	get_filename_component(filename ${_file} NAME)
	string(REGEX REPLACE "(^|/)\\+[\.|0-9]+\/${filename}" "\\1${filename}" _file "${_file}")
	set(${_output} ${_file} PARENT_SCOPE)
endfunction()

function(FILTER_VERSIONED _files _output)
	set(regex "\\+([\.|0-9]+)/[A-Z|a-z]+\.(qml|js)$")

	set(versioned_files "${_files}")
	list(FILTER versioned_files INCLUDE REGEX ${regex})
	list(SORT versioned_files COMPARE NATURAL)

	list(FILTER _files EXCLUDE REGEX ${regex})

	foreach(file ${versioned_files})
		string(REGEX MATCH ${regex} _unused "${file}")
		if(CMAKE_MATCH_1 AND QT_VERSION VERSION_LESS CMAKE_MATCH_1)
			GET_BASENAME("${file}" basefile)
			list(FIND _files "${basefile}" index)
			if(NOT index EQUAL -1)
				list(REMOVE_ITEM _files "${basefile}")
				list(APPEND _files "${file}")
			endif()
		endif()
	endforeach()

	set(${_output} ${_files} PARENT_SCOPE)
endfunction()

function(FILTER_SELECTOR _files _output _system)
	list(APPEND filterList desktop)
	list(APPEND filterList mobile)
	list(APPEND filterList ios)
	list(APPEND filterList android)

	if(_system STREQUAL "Android")
		list(REMOVE_ITEM filterList mobile)
		list(REMOVE_ITEM filterList android)
	elseif(_system STREQUAL "iOS")
		list(REMOVE_ITEM filterList mobile)
		list(REMOVE_ITEM filterList ios)
	else()
		list(REMOVE_ITEM filterList desktop)
	endif()

	foreach(file ${_files})
		set(allowed_file TRUE)
		foreach(filter ${filterList})
			if("${file}" MATCHES "/\\+${filter}/")
				set(allowed_file FALSE)
				break()
			endif()
		endforeach()

		if(allowed_file)
			list(APPEND filtered_files "${file}")
		endif()
	endforeach()

	set(${_output} ${filtered_files} PARENT_SCOPE)
endfunction()

function(GENERATE_QML_MODULES _system)
	file(GLOB DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
	foreach(name ${DIRS})
		if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${name}")
			continue()
		endif()

		file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${name}/*.qml" "${name}/*.js")

		FILTER_VERSIONED("${files}" files)
		FILTER_SELECTOR("${files}" files "${_system}")
		if("${files}" STREQUAL "")
			continue()
		endif()

		foreach(file ${files})
			file(STRINGS "${file}" PRAGMA REGEX "^pragma")
			string(REPLACE "${name}/" "" file_alias "${file}")
			GET_BASENAME("${file_alias}" file_alias)
			string(REGEX REPLACE "\\+[A-Za-z0-9_]+/" "" file_alias "${file_alias}")

			if(file_alias MATCHES "(^|/)internal/")
				set_source_files_properties("${file}" PROPERTIES QT_QML_INTERNAL_TYPE TRUE)
				string(REPLACE "internal/" "" file_alias "${file_alias}")
			endif()
			if("${PRAGMA}" MATCHES "Singleton")
				set_source_files_properties("${file}" PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
			endif()

			set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${file_alias}")
		endforeach()

		if(CMAKE_SYSTEM_NAME STREQUAL "${_system}")
			if(QT_VERSION VERSION_LESS "6.5")
				set(RESOURCE_PREFIX RESOURCE_PREFIX /qt/qml)
			endif()
			set(type general)
			set(path ${_system}/Governikus/${name})
			set(import_path ${_system})
		else()
			string(TOLOWER "${_system}" prefix)
			set(RESOURCE_PREFIX RESOURCE_PREFIX /)
			set(path ${prefix}/Governikus/${name})
			set(TARGET_PATH TARGET_PATH ${path}) # QTBUG-123169
			set(import_path ${prefix})
			set(type debug)
		endif()

		set(ModuleLibrary AusweisAppUiQmlModule${_system}${name})
		add_library(${ModuleLibrary} STATIC)
		qt_add_qml_module(${ModuleLibrary}
			URI "Governikus.${name}"
			VERSION "${PROJECT_VERSION}"
			QML_FILES ${files}
			OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${path}
			${TARGET_PATH}
			NO_CACHEGEN
			${RESOURCE_PREFIX}
			IMPORT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${import_path}
			IMPORT_PATH ${CMAKE_CURRENT_BINARY_DIR}/common
		)

		target_compile_options(${ModuleLibrary} PRIVATE "${INCOMPATIBLE_QT_COMPILER_FLAGS}")
		target_compile_options(${ModuleLibrary}plugin PRIVATE "${INCOMPATIBLE_QT_COMPILER_FLAGS}")
		target_link_libraries(AusweisAppUiQmlModules ${type} ${ModuleLibrary} ${type} ${ModuleLibrary}plugin)
	endforeach()
endfunction()

set(QML_IMPORT_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SYSTEM_NAME}" CACHE STRING "import path of ${CMAKE_SYSTEM_NAME}" FORCE)
add_library(AusweisAppUiQmlModules STATIC)
GENERATE_QML_MODULES("${CMAKE_SYSTEM_NAME}")
if(DESKTOP)
	foreach(entry Android iOS)
		GENERATE_QML_MODULES("${entry}")
	endforeach()
endif()

if(IOS)
	add_library(AusweisAppQmlPlugins INTERFACE)
	qt_import_qml_plugins(AusweisAppQmlPlugins)
	target_link_libraries(AusweisAppUiQmlModules AusweisAppQmlPlugins)
endif()
