step(${T_CFG} --preset ci-tools)

step(${T_TARGET} notes)
step(${T_TARGET} notes.latex.pdf)
step(${CMAKE_COMMAND} -E tar cfJ ../AusweisApp_ReleaseNotes.tar.xz . CHDIR ${T_BUILD_DIR}/docs/notes)

step(${T_TARGET} sdk)
step(${T_TARGET} sdk.latex.pdf)
step(${CMAKE_COMMAND} -E tar cfJ ../AusweisApp_SDK.tar.xz . CHDIR ${T_BUILD_DIR}/docs/sdk/html)

step(${T_TARGET} failurecodes)
step(${T_TARGET} failurecodes.latex.pdf)

step(${T_TARGET} installation_integration.latex.pdf)

step(${T_TARGET} license)

if(NOT RELEASE)
	step(${T_TARGET} doc8)
endif()
