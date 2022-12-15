import common.Review

build = new Review
	(
		name: 'iOS_SwiftPackage',
		label: 'iOS',
		artifacts: 'build/dist/*.zip'
	)

def j = build.generate(this)


j.with
{
	parameters
	{
		stringParam('iOS_Framework_Build', '', 'Build of iOS Framework')
		stringParam('iOS_Simulator_Framework_Build', '', 'Build of iOS Simulator-Framework')
		stringParam('iOS_Simulator_arm64_Framework_Build', '', 'Build of iOS Simulator-arm64-Framework')
	}

	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		copyArtifacts(build.getSourceJobName('iOS_Framework'))
		{
			targetDirectory('build/arm64')
			buildSelector
			{
				buildNumber('${iOS_Framework_Build}')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_Framework'))
		{
			targetDirectory('build/x86_64-simulator')
			buildSelector
			{
				buildNumber('${iOS_Simulator_Framework_Build}')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_arm64_Framework'))
		{
			targetDirectory('build/arm64-simulator')
			buildSelector
			{
				buildNumber('${iOS_Simulator_arm64_Framework_Build}')
			}
		}

		shell('cd build; cmake -P ../source/cmake/SwiftPackage.cmake')
	}
}
