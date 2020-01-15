import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'iOS_IPA',
		libraries: ['iOS'],
		label: 'iOS',
		artifacts: 'build/*.ipa,build/*.zip,build/*.bcsymbolmap'
	).generate(this)


j.with
{
	parameters
	{
		booleanParam("USE_DISTRIBUTION_PROFILE", true, "Use the provisioning profile necessary to upload AusweisApp2 to the AppStore")
	}

	steps
	{
		buildDescription('', 'USE_DISTRIBUTION_PROFILE: ${USE_DISTRIBUTION_PROFILE}')

		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell(strip('''\
			cd build;
			cmake ../source
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/iOS.toolchain.cmake
			-DUSE_DISTRIBUTION_PROFILE=\${USE_DISTRIBUTION_PROFILE}
			-GXcode
			'''))

		shell('cd build; xcodebuild -configuration MinSizeRel -archivePath AusweisApp2.xcarchive -scheme AusweisApp archive')
		shell('cd build; xcodebuild -configuration MinSizeRel -archivePath AusweisApp2.xcarchive -exportArchive -exportOptionsPlist exportOptions.plist -exportPath .')
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

				shell('cd build; xcrun altool -t ios --upload-app --verbose -u "ausweisapp@governikus.com" -p @env:PASSWORD -f *.ipa')
			}
		}
	}
}
