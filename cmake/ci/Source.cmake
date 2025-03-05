step(${T_CFG} --preset ci-tools)
step(${T_TARGET} package_source)

file(GLOB TARBALL "${T_BUILD_DIR}/*.tar.gz")
hashsum(${TARBALL})

if(RELEASE)
	step(gpg --batch --passphrase $ENV{GPG_PSW} --pinentry-mode loopback -a --detach-sig -u $ENV{GPG_ID} ${TARBALL})
endif()
