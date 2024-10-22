set(LD_ENV LD_LIBRARY_PATH=${WORKSPACE}/libs/dist/lib)

step(${T_CFG} --preset ci-bsd)
step(${T_BUILD} ENV ${LD_ENV})
step(${T_CTEST} ENV ${LD_ENV} QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins QML2_IMPORT_PATH=${WORKSPACE}/libs/dist/qml)
