import common.Review

def j = new Review
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
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('security unlock-keychain \${KEYCHAIN_CREDENTIALS} \${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-ios')

		shell('cd build; xcodebuild -configuration MinSizeRel')
		shell('cd build; xcodebuild -configuration MinSizeRel -target ipa')
	}
}
