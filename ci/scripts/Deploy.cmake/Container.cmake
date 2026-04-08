include(Utils)

file(GLOB tarball "${WORKSPACE}/*.tar")
check_beta_version(${tarball})

find_program(CMD skopeo REQUIRED)
step(${CMD} login -u $ENV{DOCKERHUB_USER} -p $ENV{DOCKERHUB_PSW} $ENV{DOCKERHUB_URL})
set(DOCKERHUB docker://$ENV{DOCKERHUB_URL}/$ENV{DOCKERHUB_IMAGE})
step(${CMD} copy docker-archive:${tarball} ${DOCKERHUB}:${PROJECT_VERSION})
step(${CMD} copy ${DOCKERHUB}:${PROJECT_VERSION} ${DOCKERHUB}:latest)
