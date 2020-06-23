FIND_PROGRAM(JSONSCHEMA_BIN NAMES jsonschema jsonschema-3 python-jsonschema python3-jsonschema py-jsonschema py3-jsonschema CMAKE_FIND_ROOT_PATH_BOTH)

IF(JSONSCHEMA_BIN)
	SET(JSON_FILES updatable-files/supported-providers.json)

	FOREACH(jsonfile ${JSON_FILES})
		SET(file ${RESOURCES_DIR}/${jsonfile})
		IF(NOT EXISTS "${file}")
			MESSAGE(FATAL_ERROR "File does not exist: ${file}")
		ENDIF()

		GET_FILENAME_COMPONENT(filename ${file} NAME)
		SET(schema ${RESOURCES_DIR}/json-schemas/${filename})

		ADD_TEST(NAME ${filename} COMMAND ${JSONSCHEMA_BIN} -i ${file} ${schema})
		SET_TESTS_PROPERTIES(${filename} PROPERTIES LABELS "json" TIMEOUT 10)
	ENDFOREACH()
ENDIF()
