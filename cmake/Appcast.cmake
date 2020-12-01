if(MAC OR LINUX OR WIN32)
	if(JENKINS_APPCAST)
		set(APPCAST_URL ${REMOTE_CONFIG_URL}/build CACHE STRING "Appcast download URL" FORCE)
	else()
		set(APPCAST_URL ${REMOTE_CONFIG_URL} CACHE STRING "Appcast download URL" FORCE)
	endif()

	macro(ADD_APPCAST_FILE _files _system _min)
		set(HASHFILE_ENDING "sha256")
		string(TIMESTAMP APPCAST_DATE "%Y-%m-%dT%H:%M:%S")

		foreach(filePath ${_files})
			if(CMAKE_VERSION VERSION_LESS "3.14")
				FILE_SIZE(fileSize ${filePath})
			else()
				file(SIZE ${filePath} fileSize)
			endif()
			get_filename_component(file ${filePath} NAME)

			if(NOT DEFINED fileSize)
				message(FATAL_ERROR "Cannot get file size of: ${file}")
			endif()

			message(STATUS "Processing: ${file}")
			if(NOT "${_system}" STREQUAL "src")
				file(READ ${PACKAGING_DIR}/updater/Appcast.item.json.in item)

				string(REPLACE "AusweisApp2-" "" APPCAST_FILE_VERSION ${file})
				string(REPLACE ".dmg" "" APPCAST_FILE_VERSION ${APPCAST_FILE_VERSION})
				string(REPLACE ".msi" "" APPCAST_FILE_VERSION ${APPCAST_FILE_VERSION})

				string(REPLACE "APPCAST_DATE" "${APPCAST_DATE}" item ${item})
				string(REPLACE "APPCAST_PLATFORM" ${_system} item ${item})
				string(REPLACE "APPCAST_MINIMUM_PLATFORM" ${_min} item ${item})
				string(REPLACE "APPCAST_VERSION" "${APPCAST_FILE_VERSION}" item ${item})
				string(REPLACE "APPCAST_URL" "${APPCAST_URL}/${file}" item ${item})
				string(REPLACE "APPCAST_SIZE" "${fileSize}" item ${item})
				string(REPLACE "APPCAST_CHECKSUM" "${APPCAST_URL}/${file}.${HASHFILE_ENDING}" item ${item})
				string(REPLACE "APPCAST_NOTES" "${APPCAST_URL}/ReleaseNotes.html#${APPCAST_FILE_VERSION}" item ${item})

				set(APPCAST_ITEMS "${APPCAST_ITEMS}${item},")
			endif()

			file(SHA256 ${filePath} fileHash)
			file(WRITE ${filePath}.${HASHFILE_ENDING} "${fileHash}  ${file}\n")
		endforeach()
	endmacro()

	if(LINUX OR MAC)
		file(GLOB DMG_FILES ${PROJECT_BINARY_DIR}/*.dmg)
		file(GLOB MSI_FILES ${PROJECT_BINARY_DIR}/*.msi)
		file(GLOB TAR_GZ_FILES ${PROJECT_BINARY_DIR}/*.tar.gz)

		if(DMG_FILES)
			ADD_APPCAST_FILE("${DMG_FILES}" "mac" "${CMAKE_OSX_DEPLOYMENT_TARGET}")
		endif()

		if(MSI_FILES)
			ADD_APPCAST_FILE("${MSI_FILES}" "win" "6.1")
		endif()

		if(TAR_GZ_FILES)
			ADD_APPCAST_FILE("${TAR_GZ_FILES}" "src" "")
		endif()

		if(APPCAST_ITEMS)
			string(REGEX REPLACE ",$" "" APPCAST_ITEMS "${APPCAST_ITEMS}")
			configure_file(${PACKAGING_DIR}/updater/Appcast.json.in ${PROJECT_BINARY_DIR}/Appcast.json @ONLY)
		endif()
	endif()

endif()
