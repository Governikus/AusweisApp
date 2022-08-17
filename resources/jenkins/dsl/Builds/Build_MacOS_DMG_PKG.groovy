import common.Build

def j = new Build
	(
		name: 'MacOS_DMG_PKG',
		libraries: ['MacOS'],
		label: 'MacOS',
		artifacts: 'build/*.dmg,build/*.pkg'
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-macos-release')

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			cmake --build build --target package --config MinSizeRel
			'''.stripIndent().trim())

		shell('cd build/_CPack_Packages/Darwin/; codesign -vvvv **/**/*.app')
		shell('cd build/_CPack_Packages/Darwin/DragNDrop; spctl -a -vv **/*.app')

		shell('cd build/; cmake -P ../source/cmake/Notarization.cmake')
	}
}
