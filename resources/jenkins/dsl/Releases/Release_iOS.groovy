import common.Release

def j = new Release
	(
		name: 'iOS_IPA',
		libraries: 'iOS',
		label: 'iOS',
		artifacts: 'libs/Toolchain_*,build/*.ipa,build/*.zip,build/*.bcsymbolmap,*.tar.zstd'
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
