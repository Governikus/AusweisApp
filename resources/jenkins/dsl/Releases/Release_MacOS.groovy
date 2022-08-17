import common.Release

def j = new Release
	(
		name: 'MacOS_DMG_PKG',
		libraries: ['MacOS'],
		label: 'MacOS',
		artifacts: 'libs/build/Toolchain_*,build/*.dmg,build/*.pkg,build/*.zip'
	).generate(this)


j.with
{
	parameters
	{
		booleanParam("UPLOAD", true, "Upload AusweisApp2 to the AppStore")
	}

	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-macos-release')

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			cmake --build build --target package --config MinSizeRel
			'''.stripIndent().trim())

		shell('cd build/src/MinSizeRel/; cmake -E tar cf ../../AusweisApp2.app.dSYM.zip --format=zip AusweisApp2.app.dSYM')

		shell('cd build/_CPack_Packages/Darwin/; codesign -vvvv **/**/*.app')
		shell('cd build/_CPack_Packages/Darwin/DragNDrop; spctl -a -vv **/*.app')

		shell('cd build/; cmake -P ../source/cmake/Notarization.cmake')

		conditionalSteps
		{
			condition
			{
				booleanCondition('${UPLOAD}')
			}

			steps
			{
				shell('cd build/; xcrun altool -t osx --upload-app -u "ausweisapp@governikus.com" -p @env:PASSWORD -f *.pkg')
			}
		}
	}
}
