if("VNC" IN_LIST NAMES)
	set(VNC ON)
	set(TARGET vnc)
	set(Dockerfile -f ${CMAKE_SOURCE_DIR}/Dockerfile.vnc)
else()
	set(TARGET sdk)
endif()

if(RELEASE)
	set(TAG ${PROJECT_VERSION})
elseif(REVIEW)
	set(TAG $ENV{CI_JOB_NAME_SLUG}-$ENV{CI_PIPELINE_ID})
else()
	set(TAG dev-${BRANCH})
	string(REPLACE "-default" "" TAG "${TAG}")
endif()

if(NOT EXISTS ${T_BUILD_DIR})
	step(${CMAKE_COMMAND} -E make_directory ${T_BUILD_DIR})
endif()

find_program(CMD buildah REQUIRED)
find_program(PODMAN podman REQUIRED)

set(IMAGE $ENV{CI_REGISTRY_IMAGE}/${TARGET}:${TAG})

step(${CMD} login -u $ENV{CI_REGISTRY_USER} -p $ENV{CI_REGISTRY_PASSWORD} $ENV{CI_REGISTRY})

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
step(${CMD} push ${IMAGE} docker-archive:${T_BUILD_DIR}/${ARTIFACT_FILENAME}-Container-${TARGET}.tar)

step(${CMD} inspect ${IMAGE})

if(RELEASE OR DAILY)
	step(${CMD} push ${IMAGE})
endif()
