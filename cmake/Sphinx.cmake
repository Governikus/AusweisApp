FIND_PROGRAM(SPHINX_BIN NAMES sphinx-build2 sphinx-build CMAKE_FIND_ROOT_PATH_BOTH)

IF(SPHINX_BIN)
	SET(SPHINX_FOUND TRUE)


	FUNCTION(SPHINX_INTERNAL_BUILD SPHINX_DOCS_DIR _target _builder _options _output _defaultLang)
		IF("${_builder}" STREQUAL "gettext")
			MESSAGE(STATUS "Generate ${_target} POT files")
		ELSE()
			FILE(GLOB _LOCALES "${SPHINX_DOCS_DIR}/locales/*")
		ENDIF()
		LIST(APPEND _LOCALES ${_defaultLang})

		SET(_TMP_DIR "${CMAKE_BINARY_DIR}/_tmp/${_target}/${_builder}")
		SET(_CONFIG_DIR "${_TMP_DIR}/config")

		SET(SPHINX_CONF_PY_IN "${SPHINX_DOCS_DIR}/${_builder}.conf.py.in")
		IF(NOT EXISTS "${SPHINX_CONF_PY_IN}")
			SET(SPHINX_CONF_PY_IN "${SPHINX_DOCS_DIR}/conf.py.in")
		ENDIF()
		CONFIGURE_FILE("${SPHINX_CONF_PY_IN}" "${_CONFIG_DIR}/conf.py" @ONLY) # Use @SPHINX_DOCS_DIR@ here

		FILE(GLOB_RECURSE _FILES
			"${SPHINX_DOCS_DIR}/*.rst"
			"${SPHINX_DOCS_DIR}/*.po"
			"${SPHINX_DOCS_DIR}/*.png"
			"${SPHINX_DOCS_DIR}/*.jpg"
			"${SPHINX_DOCS_DIR}/*.svg")

		FOREACH(lang ${_LOCALES})
			GET_FILENAME_COMPONENT(lang "${lang}" NAME)
			MESSAGE(STATUS "Generate (${_builder}) ${_target}: ${lang}")

			SET(target_dir "${_output}/${_target}/${_builder}/${lang}")
			SET(TARGET_CMD ${SPHINX_BIN} -c "${_CONFIG_DIR}" -Dlanguage=${lang} ${_options} -d "${_TMP_DIR}/${lang}" -b ${_builder} "${SPHINX_DOCS_DIR}" "${target_dir}")
			ADD_CUSTOM_COMMAND(OUTPUT ${target_dir} COMMAND ${TARGET_CMD} DEPENDS ${_FILES})
			SET(subtarget ${_target}.${_builder}.${lang})
			ADD_CUSTOM_TARGET(${subtarget} DEPENDS ${target_dir} SOURCES ${_FILES})
			LIST(APPEND target_list ${subtarget})

			IF("${_builder}" STREQUAL "latex")
				ADD_CUSTOM_TARGET(${subtarget}.pdf COMMAND make WORKING_DIRECTORY ${target_dir} DEPENDS ${subtarget})
				LIST(APPEND target_list_pdf ${subtarget}.pdf)
			ENDIF()
		ENDFOREACH()

		LIST(APPEND _FILES "${SPHINX_CONF_PY_IN}")
		ADD_CUSTOM_TARGET(${_target}.${_builder} DEPENDS ${target_list} SOURCES ${_FILES})

		IF("${_builder}" STREQUAL "latex")
			ADD_CUSTOM_TARGET(${_target}.${_builder}.pdf DEPENDS ${target_list_pdf} SOURCES ${_FILES})
		ENDIF()
	ENDFUNCTION()



	FUNCTION(SPHINX_GEN SPHINX_DOCS_DIR _target)
		SET(options )
		SET(oneValueArgs DEST OPTIONS DEFAULT_LANG)
		SET(multiValueArgs BUILDER)
		cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

		IF(NOT _PARAM_DEST)
			SET(_PARAM_DEST "${CMAKE_CURRENT_BINARY_DIR}")
		ENDIF()

		IF(NOT _PARAM_DEFAULT_LANG)
			SET(_PARAM_DEFAULT_LANG de)
		ENDIF()

		IF(NOT _PARAM_BUILDER)
			IF(SPHINX_BUILDER_${_target})
				SET(_PARAM_BUILDER ${SPHINX_BUILDER_${_target}})
			ELSEIF(SPHINX_BUILDER)
				SET(_PARAM_BUILDER ${SPHINX_BUILDER})
			ELSE()
				SET(_PARAM_BUILDER "html")
			ENDIF()
		ENDIF()

		FOREACH(builder ${_PARAM_BUILDER})
			map_get_value(builder_options "${_PARAM_OPTIONS}" "${builder}")
			SPHINX_INTERNAL_BUILD(${SPHINX_DOCS_DIR} "${_target}" "${builder}" "${builder_options}" "${_PARAM_DEST}" "${_PARAM_DEFAULT_LANG}")
			LIST(APPEND _dep_targets ${_target}.${builder})
		ENDFOREACH()

		ADD_CUSTOM_TARGET(${_target} DEPENDS ${_dep_targets})
		CREATE_DOC8_TARGET("${SPHINX_DOCS_DIR}" "${_target}")
	ENDFUNCTION()
ENDIF()
