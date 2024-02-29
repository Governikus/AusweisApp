import common.Release

def j = new Release
	(
		name: 'MacOS_DMG_PKG',
		libraries: 'MacOS',
		label: 'MacOS',
		artifacts: 'libs/Toolchain_*,build/*.dmg,build/*.pkg,build/*.zip'
	).generate(this)


j.with
{
	parameters
	{
		booleanParam("USE_DISTRIBUTION_PROFILE", true, "Use the provisioning profile necessary to upload AusweisApp to the AppStore")
	}

	steps
	{
		buildDescription('', 'USE_DISTRIBUTION_PROFILE: ${USE_DISTRIBUTION_PROFILE}')

		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-macos-release -DUSE_DISTRIBUTION_PROFILE=${USE_DISTRIBUTION_PROFILE}')

		shell('''\
			cmake --build build --target package --config MinSizeRel
			'''.stripIndent().trim())

		shell('cd build/src/MinSizeRel/; cmake -E tar cf ../../AusweisApp.app.dSYM.zip --format=zip AusweisApp.app.dSYM')

		shell('cd build/_CPack_Packages/Darwin/; codesign -vvvv **/**/*.app')
		shell('cd build/_CPack_Packages/Darwin/DragNDrop; spctl -a -vv **/*.app')

		shell('cd build/; cmake -P ../source/cmake/Notarization.cmake')

		conditionalSteps
		{
			condition
			{
				booleanCondition('${USE_DISTRIBUTION_PROFILE}')
			}

			steps
			{
				shell('cd build/; xcrun altool -t osx --upload-app -u "ausweisapp@governikus.com" -p @env:PASSWORD -f *.pkg')
			}
		}
	}
}
