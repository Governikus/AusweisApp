import common.Build

def j = new Build
	(
		name: 'iOS_Simulator_Framework',
		libraries: 'iOS_Simulator',
		label: 'iOS',
		artifacts: 'build/*.zip,build/*.bcsymbolmap'
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-ios-framework-simulator')

		shell('cd build; xcodebuild -configuration MinSizeRel')
		shell('cd build; xcodebuild -configuration MinSizeRel -target zip')
	}
}
