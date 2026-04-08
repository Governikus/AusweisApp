file(GLOB ipafile "${WORKSPACE}/*.ipa")
step(xcrun altool -t ios --validate-app --verbose -u $ENV{APPSTORE_USER} -p @env:APPSTORE_PSW -f ${ipafile})
step(xcrun altool -t ios --upload-app -u $ENV{APPSTORE_USER} -p @env:APPSTORE_PSW -f ${ipafile})
