import common.Build

def j = new Build
	(
		name: 'iOS_IPA',
		libraries: ['iOS'],
		label: 'iOS',
		artifacts: 'build/*.ipa,build/*.zip,build/*.bcsymbolmap'
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-ios')

		shell('cd build; xcodebuild -configuration MinSizeRel -archivePath AusweisApp2.xcarchive -scheme AusweisApp archive')
		shell('cd build; xcodebuild -configuration MinSizeRel -archivePath AusweisApp2.xcarchive -exportArchive -exportOptionsPlist exportOptions.plist -exportPath .')
		shell('cd build; xcodebuild -configuration MinSizeRel -target ipa')
	}
}
