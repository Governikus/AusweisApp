find_program(JSONSCHEMA_BIN NAMES jsonschema jsonschema-3 python-jsonschema python3-jsonschema py-jsonschema py3-jsonschema CMAKE_FIND_ROOT_PATH_BOTH)

if(JSONSCHEMA_BIN)
	set(JSON_FILES updatable-files/supported-providers.json updatable-files/supported-readers.json)

	foreach(jsonfile ${JSON_FILES})
		set(file ${RESOURCES_DIR}/${jsonfile})
		if(NOT EXISTS "${file}")
			message(FATAL_ERROR "File does not exist: ${file}")
		endif()

		get_filename_component(filename ${file} NAME)
		set(schema ${RESOURCES_DIR}/json-schemas/${filename})

		add_test(NAME ${filename} COMMAND ${JSONSCHEMA_BIN} -i ${file} ${schema})
		set_tests_properties(${filename} PROPERTIES LABELS "json" TIMEOUT 10)
	endforeach()
endif()
