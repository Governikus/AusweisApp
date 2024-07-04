find_program(JSONSCHEMA_BIN NAMES check-jsonschema jsonschema jsonschema-3 python-jsonschema python3-jsonschema py-jsonschema py3-jsonschema CMAKE_FIND_ROOT_PATH_BOTH)

if(JSONSCHEMA_BIN)
	set(JSON_FILES updatable-files/supported-providers.json updatable-files/supported-readers.json)

	foreach(jsonfile ${JSON_FILES})
		set(file ${RESOURCES_DIR}/${jsonfile})
		if(NOT EXISTS "${file}")
			message(FATAL_ERROR "File does not exist: ${file}")
		endif()

		get_filename_component(filename ${file} NAME)
		set(schema ${CMAKE_CURRENT_SOURCE_DIR}/${filename})

		if(JSONSCHEMA_BIN MATCHES "check-jsonschema")
			add_test(NAME ${filename} COMMAND ${JSONSCHEMA_BIN} --schemafile ${schema} ${file})
		else()
			add_test(NAME ${filename} COMMAND ${JSONSCHEMA_BIN} -i ${file} ${schema})
		endif()
		set_tests_properties(${filename} PROPERTIES LABELS "json" TIMEOUT 10)
	endforeach()
endif()


if(TARGET ${Qt}::Qml AND QT_VERSION VERSION_GREATER_EQUAL 6.5)
	add_test(NAME qmldir
			COMMAND ${CMAKE_COMMAND} -DCMD=CHECK_QMLDIR -P ${CMAKE_DIR}/cmd.cmake
			WORKING_DIRECTORY $<TARGET_PROPERTY:AusweisAppUiQmlModules,BINARY_DIR>)
	set_tests_properties(qmldir PROPERTIES LABELS "qml")

	add_test(NAME qmltypes
			COMMAND ${CMAKE_COMMAND} -DCMD=CHECK_QMLTYPES -P ${CMAKE_DIR}/cmd.cmake
			WORKING_DIRECTORY $<TARGET_PROPERTY:AusweisAppUiQmlModules,BINARY_DIR>)
	set_tests_properties(qmltypes PROPERTIES LABELS "qml")

	add_test(NAME qmllint
			COMMAND ${CMAKE_COMMAND} --build . --target all_qmllint
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
	set_tests_properties(qmllint PROPERTIES LABELS "qml")
endif()
