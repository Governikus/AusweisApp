import common.Review

def j = new Review
	(
		name: 'MacOS_DMG_PKG',
		libraries: 'MacOS',
		label: 'MacOS',
		artifacts: 'build/*.dmg,build/*.pkg'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-macos-release -DOSX_TIMESTAMP=OFF')

		shell('''\
			cmake --build build --target package --config MinSizeRel
			'''.stripIndent().trim())

		shell('cd build/_CPack_Packages/Darwin/; codesign -vvvv **/**/*.app')
		shell('cd build/_CPack_Packages/Darwin/DragNDrop; spctl -a -vv **/*.app')
	}
}
