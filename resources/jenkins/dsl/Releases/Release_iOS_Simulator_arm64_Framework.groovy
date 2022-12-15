import common.Release

def j = new Release
	(
		name: 'iOS_Simulator_arm64_Framework',
		libraries: ['iOS_Simulator_arm64'],
		label: 'iOS',
		artifacts: 'libs/build/Toolchain_*,build/*.zip,build/*.bcsymbolmap,*.tar.zstd'
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-ios-framework-simulator-arm64')

		shell('cd build; xcodebuild -configuration MinSizeRel')
		shell('cmake -E tar cf AusweisApp2_BuildDir.tar.zstd --zstd build')
		shell('cd build; xcodebuild -configuration MinSizeRel -target zip')
	}
}
