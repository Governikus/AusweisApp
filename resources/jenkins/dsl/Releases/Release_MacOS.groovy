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

		shell('cmake -P source/ci.cmake -- -DUSE_DISTRIBUTION_PROFILE=${USE_DISTRIBUTION_PROFILE}')
	}
}
