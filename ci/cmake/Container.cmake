if("VNC" IN_LIST NAMES)
	set(VNC ON)
	set(TARGET vnc)
	set(Dockerfile -f ${CMAKE_SOURCE_DIR}/Dockerfile.vnc)
else()
	set(TARGET sdk)
endif()

if(RELEASE)
	if(DEFINED ENV{GITLAB_CI})
		set(TAG $ENV{REV})
	else()
		set(TAG $ENV{changeset})
	endif()
	set(FILE_TAG ${TAG})
elseif(REVIEW)
	if(DEFINED ENV{GITLAB_CI})
		set(TAG $ENV{CI_JOB_NAME_SLUG}-$ENV{CI_PIPELINE_ID})
	else()
		set(TAG $ENV{BUILD_TAG})
	endif()
	set(FILE_TAG ${TAG})
else()
	if(DEFINED ENV{GITLAB_CI})
		set(TAG dev-$ENV{REV})
		set(FILE_TAG $ENV{REVISION})
	else()
		set(TAG dev-$ENV{MERCURIAL_REVISION_BRANCH})
		set(FILE_TAG $ENV{MERCURIAL_REVISION_SHORT})
	endif()
	string(REPLACE "-default" "" TAG "${TAG}")
endif()

if(NOT EXISTS ${T_BUILD_DIR})
	step(${CMAKE_COMMAND} -E make_directory ${T_BUILD_DIR})
endif()

if(DEFINED ENV{GITLAB_CI})
	find_program(CMD buildah REQUIRED)
	find_program(PODMAN podman REQUIRED)
else()
	find_program(CMD docker REQUIRED)
endif()

if(DAILY AND NOT VNC AND NOT DEFINED ENV{GITLAB_CI})
	step(${CMD} container prune -f)
endif()

set(IMAGE $ENV{CI_REGISTRY_IMAGE}/${TARGET}:${TAG})

step(${CMD} login -u $ENV{CI_REGISTRY_USER} -p $ENV{CI_REGISTRY_PASSWORD} $ENV{CI_REGISTRY})

if(DEFINED ENV{GITLAB_CI})
	set(CACHE_REGISTRY $ENV{CI_REGISTRY_IMAGE}/cache/${TARGET})

	step(${CMD} build
		--pull
		--layers
		--cache-from ${CACHE_REGISTRY}
		--cache-to ${CACHE_REGISTRY}
		-t ${IMAGE}
		--build-arg CCACHE_REMOTE_STORAGE=$ENV{CCACHE_REMOTE_STORAGE}
		${Dockerfile}
		${CMAKE_SOURCE_DIR}
	)
	step(${CMD} from --name ${TAG} ${IMAGE})
	step(${CMD} run ${TAG} AusweisApp --help)
	step(${CMD} rm ${TAG})
	if(NOT VNC)
		step(${PODMAN} run --rm ${IMAGE} --help)
	endif()
	step(${CMD} push ${IMAGE} docker-archive:${T_BUILD_DIR}/AusweisApp-${FILE_TAG}-Container-${TARGET}.tar)
else()
	step(${CMD} build
		--pull
		-t ${IMAGE}
		--build-arg CCACHE_REMOTE_STORAGE=$ENV{CCACHE_REMOTE_STORAGE}
		${Dockerfile}
		${CMAKE_SOURCE_DIR}
	)
	step(${CMD} run --rm ${IMAGE} AusweisApp --help)
	step(${CMD} save -o ${T_BUILD_DIR}/AusweisApp-${FILE_TAG}-Container-${TARGET}.tar ${IMAGE})
endif()

step(${CMD} inspect ${IMAGE})

if(RELEASE OR DAILY)
	step(${CMD} push ${IMAGE})
endif()


if(NOT DEFINED ENV{GITLAB_CI})
	step(${CMD} rmi -f ${IMAGE})

	if(DAILY AND NOT VNC)
		step(${CMD} images --filter "dangling=true" -q OUTPUT IMAGES)
		if(IMAGES)
			string(STRIP "${IMAGES}" IMAGES)
			string(REPLACE "\n" ";" IMAGES "${IMAGES}")
			list(REVERSE IMAGES)
			list(SUBLIST IMAGES 0 50 IMAGES)
			foreach(entry ${IMAGES})
				step(${CMD} rmi -f ${entry})
			endforeach()
		endif()
	endif()
endif()
