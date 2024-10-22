step(${T_CFG} --preset ci-linux)
step(${T_TARGET} ALL_Test_configuration)
step(${T_CTEST} -R Test_configuration ENV QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins)
step(${T_CTEST} -L json)
