CMAKE_MINIMUM_REQUIRED(VERSION 3.11.0)

###########################################
# Usage: cmake -DVERSION=5.10.0 -P qt.cmake
###########################################

IF(NOT PACKAGES_DIR)
	SET(PACKAGES_DIR $ENV{PACKAGES_DIR})
	IF(NOT PACKAGES_DIR)
		SET(PACKAGES_DIR ${CMAKE_BINARY_DIR})
	ENDIF()
ENDIF()
MESSAGE(STATUS "Use PACKAGES_DIR: ${PACKAGES_DIR}")

IF(NOT VERSION)
	MESSAGE(FATAL_ERROR "Please provide Qt version (-DVERSION=5.10.0)")
ENDIF()

FUNCTION(READ_FILE _filename _regex _out)
	FILE(STRINGS "${CMAKE_BINARY_DIR}/${_filename}" content REGEX "${_regex}")
	STRING(REGEX MATCH "${_regex}" _unused "${content}")
	SET(${_out} ${CMAKE_MATCH_1} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(FETCH_XML _url _out_url)
	SET(_filename Updates.xml)
	FILE(DOWNLOAD "${_url}/${_filename}" "${CMAKE_BINARY_DIR}/${_filename}")

	READ_FILE("${_filename}" "<DownloadableArchives>(.+)</DownloadableArchives>" archive)
	READ_FILE("${_filename}" "<Name>(.+)</Name>" name)
	READ_FILE("${_filename}" "<Version>([-|\.|0-9]+)<\/Version>" version)

	SET(${_out_url} "${_url}/${name}/${version}${archive}" PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(FETCH_HASH _url _hash_algo _out_hash)
	STRING(TOLOWER "${_hash_algo}" suffix)

	GET_FILENAME_COMPONENT(filename "${_url}" NAME)
	FILE(DOWNLOAD "${_url}.${suffix}" "${CMAKE_BINARY_DIR}/${filename}.${suffix}")
	FILE(STRINGS ${CMAKE_BINARY_DIR}/${filename}.${suffix} content)
	STRING(REGEX MATCH "^[a-z|0-9]+" hash "${content}")

	IF(NOT hash)
		MESSAGE(FATAL_ERROR "Cannot fetch hash: ${_url}.${suffix}")
	ENDIF()
	SET(${_out_hash} ${hash} PARENT_SCOPE)
ENDFUNCTION()

INCLUDE(FetchContent)
SET(FETCHCONTENT_QUIET FALSE)
SET(HASH_ALGO SHA256)
SET(QT_SDK_URL https://download.qt.io/online/qtsdkrepository)



############################ OpenSSL
SET(OPENSSL_URL ${QT_SDK_URL}/linux_x64/desktop/tools_openssl_x64)
FETCH_XML("${OPENSSL_URL}" OPENSSL_URL)
FETCH_HASH("${OPENSSL_URL}" ${HASH_ALGO} OPENSSL_HASH)

FetchContent_Populate(openssl
	URL ${OPENSSL_URL}
	URL_HASH SHA256=${OPENSSL_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}
)
FetchContent_GetProperties(openssl)

FILE(COPY "${openssl_SOURCE_DIR}/OpenSSL/binary/" DESTINATION b/${VERSION}/gcc_64)



############################ Qt
STRING(SUBSTRING ${VERSION} 0 4 SUBVERSION)
SET(QT_FILE qt-opensource-linux-x64-${VERSION}.run)
SET(QT_URL https://download.qt.io/archive/qt/${SUBVERSION}/${VERSION}/${QT_FILE})

FETCH_HASH("${QT_URL}" ${HASH_ALGO} QT_HASH)

FetchContent_Populate(qt
	URL ${QT_URL}
	URL_HASH SHA256=${QT_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}
	DOWNLOAD_NO_EXTRACT TRUE
)


SET(ENV{XDG_DATA_HOME} ${CMAKE_BINARY_DIR})
SET(ENV{XDG_DATA_DIRS} ${CMAKE_BINARY_DIR})
SET(ENV{HOME} ${CMAKE_BINARY_DIR})
STRING(REPLACE "." "" PKGVERSION "${VERSION}")
GET_FILENAME_COMPONENT(source_dir "${CMAKE_SCRIPT_MODE_FILE}" DIRECTORY)
EXECUTE_PROCESS(COMMAND chmod +x ${PACKAGES_DIR}/${QT_FILE})
EXECUTE_PROCESS(COMMAND ${PACKAGES_DIR}/${QT_FILE} --script ${source_dir}/qt-install.qs -v --platform minimal TargetDir=b Packages=qt.qt5.${PKGVERSION}.gcc_64)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink b/${VERSION}/gcc_64 dist)
