step(${T_CFG} --preset ci-tools-with-libs)
step(${CMAKE_SOURCE_DIR}/resources/jenkins/check_formatting.sh ${PENDING_PATCH} CHDIR ${CMAKE_SOURCE_DIR})
step(${CMAKE_COMMAND} -DCMD=CHECK_FAILURE_CODES -P ${CMAKE_DIR}/cmd.cmake CHDIR ${CMAKE_SOURCE_DIR})
