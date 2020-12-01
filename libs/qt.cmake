cmake_minimum_required(VERSION 3.11.0)

###########################################
# Usage: cmake -DVERSION=5.10.0 -P qt.cmake
###########################################

if(NOT PACKAGES_DIR)
	set(PACKAGES_DIR $ENV{PACKAGES_DIR})
	if(NOT PACKAGES_DIR)
		set(PACKAGES_DIR ${CMAKE_BINARY_DIR})
	endif()
endif()
message(STATUS "Use PACKAGES_DIR: ${PACKAGES_DIR}")

if(NOT VERSION)
	message(FATAL_ERROR "Please provide Qt version (-DVERSION=5.10.0)")
endif()

function(READ_FILE _filename _regex _out)
	file(STRINGS "${CMAKE_BINARY_DIR}/${_filename}" content REGEX "${_regex}")
	string(REGEX MATCH "${_regex}" _unused "${content}")
	set(${_out} ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()

function(FETCH_XML _url _out_url)
	set(_filename Updates.xml)
	file(DOWNLOAD "${_url}/${_filename}" "${CMAKE_BINARY_DIR}/${_filename}")

	READ_FILE("${_filename}" "<DownloadableArchives>(.+)</DownloadableArchives>" archive)
	READ_FILE("${_filename}" "<Name>(.+)</Name>" name)
	READ_FILE("${_filename}" "<Version>([-|\.|0-9]+)<\/Version>" version)

	set(${_out_url} "${_url}/${name}/${version}${archive}" PARENT_SCOPE)
endfunction()

function(FETCH_HASH _url _hash_algo _out_hash)
	string(TOLOWER "${_hash_algo}" suffix)

	get_filename_component(filename "${_url}" NAME)
	file(DOWNLOAD "${_url}.${suffix}" "${CMAKE_BINARY_DIR}/${filename}.${suffix}")
	file(STRINGS ${CMAKE_BINARY_DIR}/${filename}.${suffix} content)
	string(REGEX MATCH "^[a-z|0-9]+" hash "${content}")

	if(NOT hash)
		message(FATAL_ERROR "Cannot fetch hash: ${_url}.${suffix}")
	endif()
	set(${_out_hash} ${hash} PARENT_SCOPE)
endfunction()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(HASH_ALGO SHA256)
set(QT_SDK_URL https://download.qt.io/online/qtsdkrepository)



############################ OpenSSL
set(OPENSSL_URL ${QT_SDK_URL}/linux_x64/desktop/tools_openssl_x64)
FETCH_XML("${OPENSSL_URL}" OPENSSL_URL)
FETCH_HASH("${OPENSSL_URL}" ${HASH_ALGO} OPENSSL_HASH)

FetchContent_Populate(openssl
	URL ${OPENSSL_URL}
	URL_HASH SHA256=${OPENSSL_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}
)
FetchContent_GetProperties(openssl)

file(COPY "${openssl_SOURCE_DIR}/OpenSSL/binary/" DESTINATION b/${VERSION}/gcc_64)



############################ Qt
string(SUBSTRING ${VERSION} 0 4 SUBVERSION)
set(QT_FILE qt-opensource-linux-x64-${VERSION}.run)
set(QT_URL https://download.qt.io/archive/qt/${SUBVERSION}/${VERSION}/${QT_FILE})

FETCH_HASH("${QT_URL}" ${HASH_ALGO} QT_HASH)

FetchContent_Populate(qt
	URL ${QT_URL}
	URL_HASH SHA256=${QT_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}
	DOWNLOAD_NO_EXTRACT TRUE
)


set(ENV{XDG_DATA_HOME} ${CMAKE_BINARY_DIR})
set(ENV{XDG_DATA_DIRS} ${CMAKE_BINARY_DIR})
set(ENV{HOME} ${CMAKE_BINARY_DIR})
string(REPLACE "." "" PKGVERSION "${VERSION}")
get_filename_component(source_dir "${CMAKE_SCRIPT_MODE_FILE}" DIRECTORY)
execute_process(COMMAND chmod +x ${PACKAGES_DIR}/${QT_FILE})
execute_process(COMMAND ${PACKAGES_DIR}/${QT_FILE} --script ${source_dir}/qt-install.qs -v --platform minimal TargetDir=b Packages=qt.qt5.${PKGVERSION}.gcc_64)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink b/${VERSION}/gcc_64 dist)
