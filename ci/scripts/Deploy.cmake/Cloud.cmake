find_program(CURL curl REQUIRED)
list(APPEND CURL --fail -i -u $ENV{CLOUD_USER}:$ENV{CLOUD_PSW})
set(CLOUD_DESTINATION "$ENV{CLOUD_URL}/${ARTIFACT_VERSION}")

step(${CURL} -X MKCOL "${CLOUD_DESTINATION}")

file(GLOB files "${WORKSPACE}/*.msi" "${WORKSPACE}/*.dmg" "${WORKSPACE}/*.pkg" "${WORKSPACE}/*.apk" "${WORKSPACE}/*.ipa")
foreach(file ${files})
	cmake_path(GET file FILENAME filename)
	step(${CURL} -T ${file} "${CLOUD_DESTINATION}/${filename}")
endforeach()
