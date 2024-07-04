if(SPHINX_FOUND)
	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/releasenotes" "notes" BUILDER singlehtml html latex text)

	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/sdk" "sdk" BUILDER html latex DEFAULT_LANG en)

	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/installation" "installation_integration" BUILDER changes html latex)

	SPHINX_GEN("${CMAKE_CURRENT_SOURCE_DIR}/failurecodes" "failurecodes" BUILDER changes html latex DEFAULT_LANG en)
else()
	message(STATUS "No documentation will be generated")
endif()
