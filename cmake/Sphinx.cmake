find_program(SPHINX_BIN NAMES sphinx-build-2 sphinx-build2 sphinx-build sphinx-build-3 CMAKE_FIND_ROOT_PATH_BOTH)

if(SPHINX_BIN)
	set(SPHINX_FOUND TRUE)


	function(SPHINX_INTERNAL_BUILD SPHINX_DOCS_DIR _target _builder _options _output _defaultLang)
		if("${_builder}" STREQUAL "gettext")
			message(STATUS "Generate ${_target} POT files")
		else()
			file(GLOB _LOCALES "${SPHINX_DOCS_DIR}/locales/*")
		endif()
		list(APPEND _LOCALES ${_defaultLang})

		set(_TMP_DIR "${CMAKE_BINARY_DIR}/_tmp/${_target}/${_builder}")

		set(SPHINX_CONF_PY_IN "${SPHINX_DOCS_DIR}/${_builder}.conf.py.in")
		if(NOT EXISTS "${SPHINX_CONF_PY_IN}")
			set(SPHINX_CONF_PY_IN "${SPHINX_DOCS_DIR}/conf.py.in")
		endif()

		file(GLOB_RECURSE _FILES
			"${SPHINX_DOCS_DIR}/*.rst"
			"${SPHINX_DOCS_DIR}/*.po"
			"${SPHINX_DOCS_DIR}/*.png"
			"${SPHINX_DOCS_DIR}/*.jpg"
			"${SPHINX_DOCS_DIR}/*.svg")

		foreach(SPHINX_LANG ${_LOCALES})
			get_filename_component(SPHINX_LANG "${SPHINX_LANG}" NAME)
			message(STATUS "Generate (${_builder}) ${_target}: ${SPHINX_LANG}")

			set(_CONFIG_DIR "${_TMP_DIR}/config/${SPHINX_LANG}")
			configure_file("${SPHINX_CONF_PY_IN}" "${_CONFIG_DIR}/conf.py" @ONLY) # Use @SPHINX_DOCS_DIR@ / @SPHINX_LANG@ here

			set(target_dir "${_output}/${_target}/${_builder}/${SPHINX_LANG}")
			set(TARGET_CMD ${SPHINX_BIN} -c "${_CONFIG_DIR}" ${_options} -d "${_TMP_DIR}/${SPHINX_LANG}" -b ${_builder} "${SPHINX_DOCS_DIR}" "${target_dir}")
			add_custom_command(OUTPUT ${target_dir} COMMAND ${TARGET_CMD} DEPENDS ${_FILES})
			set(subtarget ${_target}.${_builder}.${SPHINX_LANG})
			add_custom_target(${subtarget} DEPENDS ${target_dir} SOURCES ${_FILES})
			list(APPEND target_list ${subtarget})

			if("${_builder}" STREQUAL "latex")
				if(WIN32)
					set(MAKE_BIN make.bat)
				else()
					find_program(MAKE_BIN NAMES gmake make)
				endif()
				add_custom_target(${subtarget}.pdf COMMAND ${MAKE_BIN} WORKING_DIRECTORY ${target_dir} DEPENDS ${subtarget})
				list(APPEND target_list_pdf ${subtarget}.pdf)
			endif()
		endforeach()

		list(APPEND _FILES "${SPHINX_CONF_PY_IN}")
		add_custom_target(${_target}.${_builder} DEPENDS ${target_list} SOURCES ${_FILES})

		if("${_builder}" STREQUAL "latex")
			add_custom_target(${_target}.${_builder}.pdf DEPENDS ${target_list_pdf} SOURCES ${_FILES})
		endif()
	endfunction()



	function(SPHINX_GEN SPHINX_DOCS_DIR _target)
		set(options )
		set(oneValueArgs DEST OPTIONS DEFAULT_LANG)
		set(multiValueArgs BUILDER)
		cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

		if(NOT _PARAM_DEST)
			set(_PARAM_DEST "${CMAKE_CURRENT_BINARY_DIR}")
		endif()

		if(NOT _PARAM_DEFAULT_LANG)
			set(_PARAM_DEFAULT_LANG de)
		endif()

		if(NOT _PARAM_BUILDER)
			if(SPHINX_BUILDER_${_target})
				set(_PARAM_BUILDER ${SPHINX_BUILDER_${_target}})
			elseif(SPHINX_BUILDER)
				set(_PARAM_BUILDER ${SPHINX_BUILDER})
			else()
				set(_PARAM_BUILDER "html")
			endif()
		endif()

		foreach(builder ${_PARAM_BUILDER})
			map_get_value(builder_options "${_PARAM_OPTIONS}" "${builder}")
			SPHINX_INTERNAL_BUILD(${SPHINX_DOCS_DIR} "${_target}" "${builder}" "${builder_options}" "${_PARAM_DEST}" "${_PARAM_DEFAULT_LANG}")
			list(APPEND _dep_targets ${_target}.${builder})
		endforeach()

		add_custom_target(${_target} DEPENDS ${_dep_targets})
		CREATE_DOC8_TARGET("${SPHINX_DOCS_DIR}" "${_target}")
	endfunction()
endif()
