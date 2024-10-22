step(${T_CFG} --preset ci-tools)
step(${T_TARGET} package_source)

if(RELEASE)
	file(GLOB TARBALL "${T_BUILD_DIR}/*.tar.gz")
	step(gpg --batch --passphrase $ENV{GPG_PSW} --pinentry-mode loopback -a --detach-sig -u $ENV{GPG_ID} ${TARBALL})
endif()
