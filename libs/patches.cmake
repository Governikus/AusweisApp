cmake_minimum_required(VERSION 3.19.0)

# How to use
#
# Create an empty directory and provide it by -DREPOSITORY_DIR or set
# REPOSITORY_DIR as environment variable.
#
# - Apply existing patches
# $ cmake -DCMD=apply -P libs/patches.cmake
# 1. This will clone all repositores to REPOSITORY_DIR.
# 2. Checkout git tag from Versions.cmake to a new ausweisap_ branch.
#    It will delete that branch if it exist.
# 3. Apply all patches to the new branch.
#
# - Generate patches from repositories
# $ cmake -DCMD=generate -P libs/patches.cmake
# 1. All existing patches in "patches" directory will be deleted.
# 2. Branch of current Version on all repositories in REPOSITORY_DIR will be scanned.
# 3. All changesets from the latest tag will be exported to "patches" dir.
#
# If you need a new repository you can clone it into REPOSITORY_DIR.
# Checkout the release tag of used Version.cmake and cherry-pick or add
# changesets to it. It will be exported with "generate".
# If you need to remove patches, just delete the changesets or the whole clone.
#
# - Upgrade Qt or OpenSSL
# 1. Apply all patches with this script and do the following on each repository.
#    The version is an example and should be adjusted!
# 2. git checkout ausweisapp_6.4.3 -b ausweisapp_6.6.0
# 3. git rebase --onto v6.6.0 v6.4.3 HEAD
# 4. Bump version in Versions.cmake and use this script to generate the patches.

if(NOT CMAKE_SCRIPT_MODE_FILE OR NOT CMD)
	message(FATAL_ERROR "Usage: cmake -DCMD=apply|generate -P libs/patches.cmake")
endif()

find_package(Git REQUIRED)

if(NOT REPOSITORY_DIR)
	set(REPOSITORY_DIR $ENV{REPOSITORY_DIR})
	if(NOT REPOSITORY_DIR)
		message(FATAL_ERROR "Define REPOSITORY_DIR for local repositories")
	endif()
	if(NOT EXISTS "${REPOSITORY_DIR}")
		message(FATAL_ERROR "REPOSITORY_DIR ${REPOSITORY_DIR} does not exist")
	endif()
endif()

get_filename_component(libs_dir "${CMAKE_SCRIPT_MODE_FILE}" DIRECTORY)
set(patch_dir "${libs_dir}/patches")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake;${libs_dir}")
include(Versions)

function(execute_dir dir)
	execute_process(COMMAND ${GIT_EXECUTABLE} ${ARGN}
		WORKING_DIRECTORY "${REPOSITORY_DIR}/${dir}"
		RESULT_VARIABLE _result)

	if(NOT "${_result}" EQUAL 0)
		message(FATAL_ERROR "git failed: ${_result}")
	endif()
endfunction()

function(execute)
	execute_dir("" ${ARGN})
endfunction()

function(git_clone prefix)
	if(NOT EXISTS "${REPOSITORY_DIR}/${prefix}")
		message(STATUS "Repository not found: ${prefix}")
		if(prefix STREQUAL "openssl")
			execute(clone "https://github.com/openssl/openssl" "${prefix}")
		elseif(prefix MATCHES "qt")
			execute(clone "https://code.qt.io/qt/${prefix}.git" "${prefix}")
		endif()
	endif()
endfunction()

function(git_checkout prefix)
	get_version_branch("${prefix}" version tmp_branch)
	execute_dir("${prefix}" fetch)
	execute_dir("${prefix}" checkout -q ${version})
	execute_process(COMMAND ${GIT_EXECUTABLE} branch -D ${tmp_branch} WORKING_DIRECTORY ${REPOSITORY_DIR}/${prefix})
	execute_process(COMMAND ${GIT_EXECUTABLE} am --abort WORKING_DIRECTORY ${REPOSITORY_DIR}/${prefix} OUTPUT_QUIET ERROR_QUIET)
	execute_dir("${prefix}" checkout ${version} -b ${tmp_branch})
endfunction()

function(get_version_branch prefix _version _branch)
	set(tmp_branch ausweisapp)
	if(prefix STREQUAL "openssl")
		set(version openssl-${OPENSSL})
		set(tmp_branch ${tmp_branch}_${OPENSSL})
	elseif(prefix MATCHES "qt")
		set(version v${QT})
		set(tmp_branch ${tmp_branch}_${QT})
	endif()

	set(${_version} "${version}" PARENT_SCOPE)
	set(${_branch} "${tmp_branch}" PARENT_SCOPE)
endfunction()

function(apply_patches)
	file(GLOB PATCHES "${patch_dir}/*.patch")
	foreach(patch ${PATCHES})
		get_filename_component(filename "${patch}" NAME)
		string(REGEX MATCH "([a-z|-]+)-[0-9]+.+" _unused "${filename}")
		set(prefix ${CMAKE_MATCH_1})

		list(APPEND prefixes "${prefix}")
		list(APPEND "${prefix}" "${patch}")

		if(NOT (prefix STREQUAL "openssl" OR prefix MATCHES "qt"))
			message(FATAL_ERROR "Prefix unknown: ${prefix}")
		endif()
	endforeach()

	list(REMOVE_DUPLICATES prefixes)
	foreach(prefix ${prefixes})
		message(STATUS "Apply component: ${prefix}")
		git_clone(${prefix})
		git_checkout(${prefix})

		foreach(patch ${${prefix}})
			if(prefix STREQUAL "openssl")
				set(p 1)
			elseif(prefix MATCHES "qt")
				set(p 2)
			endif()

			execute_process(COMMAND ${CMAKE_COMMAND} -E env GIT_COMMITTER_NAME="Governikus" GIT_COMMITTER_EMAIL="" --
				${GIT_EXECUTABLE} am --whitespace=fix --no-gpg-sign --committer-date-is-author-date -p${p} "${patch}"
				WORKING_DIRECTORY "${REPOSITORY_DIR}/${prefix}"
				RESULT_VARIABLE _result)

			if(NOT "${_result}" EQUAL 0)
				message(FATAL_ERROR "cannot apply patch: ${_result}")
			endif()
		endforeach()
	endforeach()
endfunction()


function(get_latest_tag _out repo)
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
		WORKING_DIRECTORY ${repo}
		OUTPUT_VARIABLE _output
		RESULT_VARIABLE _result
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	if(${_result} EQUAL 0)
		set(${_out} "${_output}" PARENT_SCOPE)
	endif()
endfunction()

function(get_current_branch _out repo)
	execute_process(COMMAND ${GIT_EXECUTABLE} branch --show-current
		WORKING_DIRECTORY ${repo}
		OUTPUT_VARIABLE _output
		RESULT_VARIABLE _result
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	if(NOT "${_result}" EQUAL 0)
		message(FATAL_ERROR "git failed: ${_result}")
	endif()
	set(${_out} "${_output}" PARENT_SCOPE)
endfunction()

function(rename_patches component)
	file(GLOB PATCHES "${patch_dir}/*.patch")
	foreach(patch ${PATCHES})
		get_filename_component(filename "${patch}" NAME)
		if(filename MATCHES "^[0-9]+.*")
			file(RENAME "${patch}" "${patch_dir}/${component}-${filename}")
		endif()
	endforeach()
endfunction()

function(generate_patches)
	file(GLOB PATCHES "${patch_dir}/*.patch")
	if(PATCHES)
		file(REMOVE ${PATCHES})
	endif()

	list(APPEND prefixes openssl qt)
	foreach(prefix ${prefixes})
		file(GLOB REPOS "${REPOSITORY_DIR}/${prefix}*")
		foreach(repo ${REPOS})
			get_filename_component(dirname "${repo}" NAME)
			get_version_branch("${prefix}" version tmp_branch)
			execute_process(COMMAND ${GIT_EXECUTABLE} checkout -q ${tmp_branch} WORKING_DIRECTORY ${repo} OUTPUT_QUIET ERROR_QUIET)
			get_current_branch(current_branch "${repo}")
			if(current_branch STREQUAL tmp_branch)
				get_latest_tag(latesttag "${repo}")
				message(STATUS "Generate patches of ${dirname} since tag ${latesttag}")

				if(dirname STREQUAL "openssl")
					set(component "")
				else()
					set(component "${dirname}/")
				endif()

				execute_dir("${dirname}" format-patch --no-signature --no-renames --no-binary --src-prefix=x/${component} --dst-prefix=y/${component} -k ${latesttag}..HEAD -o "${patch_dir}")
				rename_patches(${dirname})
			else()
				message(STATUS "Skip patches of ${dirname} and branch ${current_branch}")
			endif()
		endforeach()
	endforeach()
endfunction()



if(CMD STREQUAL "generate")
	message(STATUS "Generate patches!")
	generate_patches()
elseif(CMD STREQUAL "apply")
	message(STATUS "Apply patches!")
	apply_patches()
else()
	message(FATAL_ERROR "Unknown CMD: ${CMD}")
endif()
