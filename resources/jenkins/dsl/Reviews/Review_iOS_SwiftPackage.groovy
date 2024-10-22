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
		copyArtifacts(build.getSourceJobName('iOS_Framework'))
		{
			targetDirectory('arm64')
			flatten()
			buildSelector
			{
				buildNumber('${iOS_Framework_Build}')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_Framework'))
		{
			targetDirectory('x86_64-simulator')
			flatten()
			buildSelector
			{
				buildNumber('${iOS_Simulator_Framework_Build}')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_arm64_Framework'))
		{
			targetDirectory('arm64-simulator')
			flatten()
			buildSelector
			{
				buildNumber('${iOS_Simulator_arm64_Framework_Build}')
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
