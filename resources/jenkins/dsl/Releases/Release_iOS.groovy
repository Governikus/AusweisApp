import common.Release

def j = new Release
	(
		name: 'iOS_IPA',
		libraries: ['iOS'],
		label: 'iOS',
		artifacts: 'libs/build/Toolchain_*,build/*.ipa,build/*.zip,build/*.bcsymbolmap,*.tar.zstd'
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

		shell('cd source; cmake --preset ci-ios -DUSE_DISTRIBUTION_PROFILE=${USE_DISTRIBUTION_PROFILE}')

		shell('cd build; xcodebuild -configuration MinSizeRel -archivePath AusweisApp.xcarchive -scheme AusweisAppBinary archive')
		shell('cd build; xcodebuild -configuration MinSizeRel -archivePath AusweisApp.xcarchive -exportArchive -exportOptionsPlist exportOptions.plist -exportPath .')
		shell('cmake -E tar cf AusweisApp_BuildDir.tar.zstd --zstd build')
		shell('cd build; xcodebuild -configuration MinSizeRel -target ipa')

		conditionalSteps
		{
			condition
			{
				booleanCondition('${USE_DISTRIBUTION_PROFILE}')
			}

			steps
			{
				shell('cd build; xcrun altool -t ios --validate-app --verbose -u "ausweisapp@governikus.com" -p @env:PASSWORD -f *.ipa')

				shell('cd build; xcrun altool -t ios --upload-app -u "ausweisapp@governikus.com" -p @env:PASSWORD -f *.ipa')
			}
		}
	}
}
