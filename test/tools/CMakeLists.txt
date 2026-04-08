find_program(JSONSCHEMA_BIN NAMES check-jsonschema)

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
		set_tests_properties(${filename} PROPERTIES LABELS "json" TIMEOUT 30)
	endforeach()
endif()


if(TARGET ${Qt}::Qml)
	add_test(NAME qmldir
			COMMAND ${CMAKE_COMMAND} -DCMD=CHECK_QMLDIR -P ${CMAKE_DIR}/cmd.cmake
			WORKING_DIRECTORY $<TARGET_PROPERTY:AusweisAppUiQmlModules,BINARY_DIR>)
	set_tests_properties(qmldir PROPERTIES LABELS "qml")

	add_test(NAME qmltypes
			COMMAND ${CMAKE_COMMAND} -DCMD=CHECK_QMLTYPES -P ${CMAKE_DIR}/cmd.cmake
			WORKING_DIRECTORY $<TARGET_PROPERTY:AusweisAppUiQmlModules,BINARY_DIR>)
	set_tests_properties(qmltypes PROPERTIES LABELS "qml")

	add_test(NAME qmlenums
			COMMAND ${CMAKE_COMMAND} -DCMD=CHECK_QMLENUMS -P ${CMAKE_DIR}/cmd.cmake
			WORKING_DIRECTORY $<TARGET_PROPERTY:AusweisAppUiQmlModules,SOURCE_DIR>)
	set_tests_properties(qmltypes PROPERTIES LABELS "qml")

	if(Python_FOUND)
		add_test(NAME qmlresources
				COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/checkQmlResources.py
						--module-dir=${CMAKE_SOURCE_DIR}/src/ui/qml/modules
						--exclude-filenames=App,Workflow
						--resources-dir=${CMAKE_SOURCE_DIR}/resources
						--exclude-resources=${CMAKE_CURRENT_SOURCE_DIR}/checkQmlResources_excludes.txt)
		set_tests_properties(qmlresources PROPERTIES LABELS "qml")
	endif()

	add_subdirectory(qmllint)
endif()


find_program(TYPOS_BIN typos)
if(TYPOS_BIN)
	add_test(NAME typos
			COMMAND ${TYPOS_BIN}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	set_tests_properties(typos PROPERTIES LABELS "sanity")
endif()

find_program(CODESPELL_BIN codespell)
if(CODESPELL_BIN)
	add_test(NAME codespell
			COMMAND ${CODESPELL_BIN}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	set_tests_properties(codespell PROPERTIES LABELS "sanity")
endif()

find_program(SPHINX_LINT_BIN sphinx-lint)
if(SPHINX_LINT_BIN)
	add_test(NAME sphinx-lint
			COMMAND ${SPHINX_LINT_BIN} -e all --max-line-length 120 -i LC_MESSAGES
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	set_tests_properties(codespell PROPERTIES LABELS "sanity")
endif()

find_program(YAMLLINT_BIN yamllint)
if(YAMLLINT_BIN)
	add_test(NAME yamllint
			COMMAND ${YAMLLINT_BIN} --strict ${CMAKE_SOURCE_DIR}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	set_tests_properties(yamllint PROPERTIES LABELS "sanity")
endif()

if(NOT RUFF) # See Tools.cmake
	find_program(RUFF ruff)
endif()
if(RUFF)
	add_test(NAME ruff
			COMMAND ${RUFF} check ${CMAKE_SOURCE_DIR}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	set_tests_properties(ruff PROPERTIES LABELS "sanity")
endif()

if(VENDOR_GOVERNIKUS AND LIBS_GOVERNIKUS)
	find_program(GRYPE grype OPTIONAL)
	if(GRYPE)
		if(DEFINED ENV{CI})
			set(GRYPE_CONFIG CONFIGURATIONS periodic)
		endif()

		set(cmd ${GRYPE} -c ${CMAKE_SOURCE_DIR}/.grype.yml --show-suppressed -f low)
		add_test(NAME grype COMMAND ${cmd} ${QT6_INSTALL_PREFIX} ${GRYPE_CONFIG})
		set_tests_properties(grype PROPERTIES LABELS "grype")

		set(regex "^ExternalDocumentRef:.*")
		file(GLOB SPDX "${QT6_INSTALL_PREFIX}/sbom/*.spdx")
		foreach(entry ${SPDX})
			# https://github.com/anchore/grype/issues/3107
			file(STRINGS "${entry}" HAS_EXTERNAL_DOCUMENT_REF REGEX "${regex}")
			if(HAS_EXTERNAL_DOCUMENT_REF)
				file(STRINGS ${entry} LINES)
				file(WRITE ${entry} "")
				foreach(line IN LISTS LINES)
					string(REGEX REPLACE ${regex} "" STRIPPED "${line}")
					file(APPEND ${entry} "${STRIPPED}\n")
				endforeach()
			endif()

			cmake_path(GET entry FILENAME filename)
			set(testname grype.${filename})
			add_test(NAME ${testname} COMMAND ${cmd} sbom:${entry} ${GRYPE_CONFIG})
			set_tests_properties(${testname} PROPERTIES LABELS "grype")
			set_tests_properties(${testname} PROPERTIES DEPENDS "grype")
		endforeach()
	endif()
endif()
