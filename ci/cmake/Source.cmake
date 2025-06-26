step(${T_CFG} --preset ci-tools)
step(${T_TARGET} package_source)

file(GLOB TARBALL "${T_BUILD_DIR}/*.tar.gz")
hashsum(${TARBALL})

block()
	include(Files)
endblock()
prepare_gpg(GPG_CMD)
step(${GPG_CMD} -a --detach-sig -u $ENV{GPG_ID} ${TARBALL})
