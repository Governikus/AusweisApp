import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'iOS_IPA',
		libraries: 'iOS',
		label: 'iOS',
		artifacts: 'build/*.ipa'
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
			-DCMAKE_BUILD_TYPE=release
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/iOS.toolchain.cmake
			-DUSE_DISTRIBUTION_PROFILE=\${USE_DISTRIBUTION_PROFILE}
			-GXcode
			'''))

		shell('cd build; xcodebuild -target install -configuration Release ARCHS=arm64')
		shell('cd build; xcodebuild -target ipa -configuration Release')
	}
}
