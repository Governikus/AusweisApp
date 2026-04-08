file(GLOB pkgfile "${WORKSPACE}/*.pkg")
step(xcrun altool -t osx --upload-app -u $ENV{APPSTORE_USER} -p @env:APPSTORE_PSW -f ${pkgfile})
