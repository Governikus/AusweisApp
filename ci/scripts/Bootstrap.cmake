set(ENV_FILE ${CMAKE_BINARY_DIR}/env)

function(append_env_file _line)
	file(APPEND "${ENV_FILE}" ${_line}\n)
endfunction()

step(hg log -r . -T {node} -R ${CMAKE_SOURCE_DIR} OUTPUT REVISION)

append_env_file("RELEASE_FINAL=${IS_FINAL_VERSION}")
append_env_file("RELEASE=${RELEASE}")
append_env_file("REVISION=${REVISION}")

configure_file("${CMAKE_SOURCE_DIR}/.gitlab-ci-child.yml" "${CMAKE_BINARY_DIR}/gitlab-ci-child.yml" COPYONLY)
