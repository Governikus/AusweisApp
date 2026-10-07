set(ENV_FILE ${CMAKE_BINARY_DIR}/env)

function(append_env_file _line)
	file(APPEND "${ENV_FILE}" ${_line}\n)
endfunction()

step(hg log -r . -T {node} -R ${CMAKE_SOURCE_DIR} OUTPUT REVISION)

append_env_file("RELEASE_FINAL=${IS_FINAL_VERSION}")
append_env_file("RELEASE=${RELEASE}")
append_env_file("REVISION=${REVISION}")


function(PREPARE_FOR_YAML _in_out _indent)
	string(REPEAT " " ${_indent} padding)
	foreach(entry IN LISTS ${_in_out})
		list(APPEND padded "${padding}- ${entry}")
	endforeach()
	string(JOIN "\n" padded ${padded})
	set(${_in_out} ${padded} PARENT_SCOPE)
endfunction()

FETCH_VERSION_LIBS()
list(REMOVE_ITEM OPENSSL_VERSIONS ${OPENSSL})
PREPARE_FOR_YAML(OPENSSL_VERSIONS 10)

configure_file("${CMAKE_SOURCE_DIR}/.gitlab-ci-child.yml.in" "${CMAKE_BINARY_DIR}/gitlab-ci-child.yml" @ONLY)
