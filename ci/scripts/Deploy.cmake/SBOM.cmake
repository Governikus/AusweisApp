file(GLOB SBOM "${WORKSPACE}/*.spdx")
list(LENGTH SBOM SBOM_COUNT)
if(NOT SBOM_COUNT EQUAL 1)
	message(FATAL_ERROR "Cannot find unique SBOM file: ${SBOM}")
endif()

set(SBOM_JSON ${SBOM}.json)

find_program(SYFT syft REQUIRED)
step(${SYFT} convert ${SBOM} -o cyclonedx-json=${SBOM_JSON})

if(IS_FINAL_VERSION)
	set(IS_LATEST true)
else()
	set(IS_LATEST false)
endif()

find_program(CURL curl REQUIRED)
step(${CURL} --fail -i
	"$ENV{DEPENDENCY_TRACK_API_URL}/v1/bom"
	-H "X-Api-Key: $ENV{DEPENDENCY_TRACK_TOKEN}"
	-F "parentUUID=$ENV{DEPENDENCY_TRACK_PROJECT_ID}"
	-F "projectName=${PROJECT_NAME}"
	-F "projectVersion=${ARTIFACT_VERSION}"
	-F "autoCreate=true"
	-F "isLatest=${IS_LATEST}"
	-F "bom=@${SBOM_JSON}")
