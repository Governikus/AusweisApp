if(NAME MATCHES "VNC")
	set(VNC ON)
	set(TARGET vnc)
	set(Dockerfile -f ${CMAKE_SOURCE_DIR}/resources/jenkins/docker/Dockerfile)
else()
	set(TARGET sdk)
endif()

if(RELEASE)
	set(TAG $ENV{changeset})
	set(FILE_TAG ${TAG})
elseif(REVIEW)
	set(TAG $ENV{BUILD_TAG})
	set(FILE_TAG ${TAG})
else()
	set(TAG dev-$ENV{MERCURIAL_REVISION_BRANCH})
	string(REPLACE "-default" "" TAG "${TAG}")
	set(FILE_TAG $ENV{MERCURIAL_REVISION_SHORT})
endif()

if(NOT EXISTS ${T_BUILD_DIR})
	step(${CMAKE_COMMAND} -E make_directory ${T_BUILD_DIR})
endif()

if(NOT VNC)
	step(docker container prune -f)
endif()

step(docker build --pull
	-t dev-docker.govkg.de/ausweisapp/${TARGET}:${TAG}
	--build-arg CCACHE_REMOTE_STORAGE=redis://$ENV{CCACHE_REMOTE_STORAGE_HOST}
	${Dockerfile}
	${CMAKE_SOURCE_DIR}
)

step(docker run --rm dev-docker.govkg.de/ausweisapp/${TARGET}:${TAG} AusweisApp --help)

step(docker save -o ${T_BUILD_DIR}/AusweisApp-${FILE_TAG}.tar dev-docker.govkg.de/ausweisapp/${TARGET}:${TAG})

if(REVIEW)
	step(docker rmi -f dev-docker.govkg.de/ausweisapp/${TARGET}:${TAG})
else()
	step(docker push dev-docker.govkg.de/ausweisapp/${TARGET}:${TAG})
endif()


if(DAILY AND NOT VNC)
	step(docker images --filter "dangling=true" -q OUTPUT IMAGES)
	if(IMAGES)
		string(STRIP "${IMAGES}" IMAGES)
		string(REPLACE "\n" ";" IMAGES "${IMAGES}")
		list(REVERSE IMAGES)
		list(SUBLIST IMAGES 0 50 IMAGES)
		foreach(entry ${IMAGES})
			step(docker rmi -f ${entry})
		endforeach()
	endif()
endif()


if(RELEASE AND DEFINED ENV{LATEST})
	if($ENV{LATEST})
		step(docker tag dev-docker.govkg.de/ausweisapp/${TARGET}:${TAG} dev-docker.govkg.de/ausweisapp/${TARGET}:latest)
		step(docker push dev-docker.govkg.de/ausweisapp/${TARGET}:latest)
	endif()
endif()
