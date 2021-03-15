if(SPHINX_FOUND)
	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/releasenotes" "notes" BUILDER singlehtml html latex text)

	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/sdk" "sdk" BUILDER changes html latex DEFAULT_LANG en)

	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/installation" "inst" BUILDER changes html latex)

	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/integration" "inte" BUILDER changes html latex)
else()
	message(STATUS "No documentation will be generated")
endif()
