function(PANDOC_RST_TO_PDF DOCS_DIR _target)
	set(options)
	set(oneValueArgs OUTPUT_NAME TRANSLATE)
	set(multiValueArgs CUSTOM_PANDOC_PARAM)
	cmake_parse_arguments(PANDOC_RST_TO_PDF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(CMAKE_PANDOC_DIR "${CMAKE_DIR}/pandoc")
	set(PREPROCESS_SCRIPT "${CMAKE_PANDOC_DIR}/preprocess_rst.py")

	set(BUILD_OUTPUT_DIR "${CMAKE_BINARY_DIR}/docs/${_target}")
	file(MAKE_DIRECTORY "${BUILD_OUTPUT_DIR}")

	# Configure metadata.yaml
	if(PANDOC_RST_TO_PDF_TRANSLATE)
		configure_file("${DOCS_DIR}/metadata_${PANDOC_RST_TO_PDF_TRANSLATE}.yaml.in" "${BUILD_OUTPUT_DIR}/metadata.yaml" @ONLY)
	else()
		configure_file("${DOCS_DIR}/metadata.yaml.in" "${BUILD_OUTPUT_DIR}/metadata.yaml" @ONLY)
	endif()

	# Find lua filters
	file(GLOB_RECURSE FILTERS "${CMAKE_PANDOC_DIR}/*.lua")
	foreach(FILTER IN LISTS FILTERS)
		list(APPEND CUSTOM_LUA_FILTERS "--lua-filter=${FILTER}")
	endforeach()

	# Copy pregenerated PDFs
	file(GLOB_RECURSE INPUT_PDF_FILES "${DOCS_DIR}/*.pdf")
	foreach(INPUT_PDF IN LISTS INPUT_PDF_FILES)
		get_filename_component(PDF_FILENAME "${INPUT_PDF}" NAME_WLE)
		set(OUTPUT_PDF "${BUILD_OUTPUT_DIR}/${PDF_FILENAME}.pdf")
		set(TARGET_NAME "docs_${_target}_${PDF_FILENAME}.pdf")
		add_custom_command(
			OUTPUT "${OUTPUT_PDF}"
			COMMAND ${CMAKE_COMMAND} -E copy "${INPUT_PDF}" "${OUTPUT_PDF}"
			DEPENDS "${INPUT_PDF}"
			VERBATIM
		)
		add_custom_target("${TARGET_NAME}"
			DEPENDS "${OUTPUT_PDF}"
		)
		list(APPEND _dep_targets "${TARGET_NAME}")
	endforeach()

	if(PANDOC_RST_TO_PDF_TRANSLATE)
		set(PYTHON_SCRIPT_TRANSLATE_ARGUMENT "--translate=${PANDOC_RST_TO_PDF_TRANSLATE}")
	else()
		set(PYTHON_SCRIPT_TRANSLATE_ARGUMENT "")
	endif()

	# Preprocess index file
	set(INPUT_RST "${DOCS_DIR}/index.rst")
	set(OUTPUT_RST "${BUILD_OUTPUT_DIR}/index.rst")
	add_custom_command(
		OUTPUT "${OUTPUT_RST}"
		COMMAND
			"${Python_EXECUTABLE}"
			"${PREPROCESS_SCRIPT}"
			"${INPUT_RST}"
			"${OUTPUT_RST}"
			"${PROJECT_NAME}"
			"${PROJECT_VERSION}"
			${PYTHON_SCRIPT_TRANSLATE_ARGUMENT}
		DEPENDS
			"${INPUT_RST}"
			"${PREPROCESS_SCRIPT}"
		WORKING_DIRECTORY "${BUILD_OUTPUT_DIR}"
		VERBATIM
	)
	add_custom_target("docs_${_target}_index_rst" DEPENDS "${OUTPUT_RST}")
	list(APPEND _dep_targets "docs_${_target}_index_rst")

	# Collect rst files index.rst depends on
	file(GLOB_RECURSE RST_FILES "${DOCS_DIR}/*.rst")
	list(APPEND _dep_targets ${RST_FILES})

	# Convert index to final pdf
	set(TARGET_NAME "${_target}.latex.pdf")
	set(OUTPUT_PATH "${BUILD_OUTPUT_DIR}/${ARTIFACT_FILENAME}-${PANDOC_RST_TO_PDF_OUTPUT_NAME}.pdf")
	add_custom_command(
		OUTPUT "${OUTPUT_PATH}"
		COMMAND
			${PANDOC_BIN}
			"--metadata-file=metadata.yaml"
			"--template=${CMAKE_PANDOC_DIR}/template.tex"
			${CUSTOM_LUA_FILTERS}
			${PANDOC_RST_TO_PDF_CUSTOM_PANDOC_PARAM}
			"${OUTPUT_RST}"
			--to=pdf
			"--output=${OUTPUT_PATH}"
		DEPENDS
			${_dep_targets}
			${OUTPUT_RST}
			${FILTERS}
			"${PREPROCESS_SCRIPT}"
			"${BUILD_OUTPUT_DIR}/metadata.yaml"
			"${CMAKE_PANDOC_DIR}/template.tex"
		WORKING_DIRECTORY "${BUILD_OUTPUT_DIR}"
		VERBATIM
	)
	add_custom_target("${TARGET_NAME}" DEPENDS "${OUTPUT_PATH}")
endfunction()
