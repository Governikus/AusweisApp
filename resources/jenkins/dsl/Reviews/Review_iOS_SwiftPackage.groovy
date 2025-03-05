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
		stringParam('iOS_Framework_Simulator_Build', '', 'Build of iOS Framework for Simulator')
		stringParam('iOS_Framework_Simulator_arm64_Build', '', 'Build of iOS Framework for Simulator-arm64')
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

		copyArtifacts(build.getSourceJobName('iOS_Framework_Simulator'))
		{
			targetDirectory('x86_64-simulator')
			flatten()
			buildSelector
			{
				buildNumber('${iOS_Framework_Simulator_Build}')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Framework_Simulator_arm64'))
		{
			targetDirectory('arm64-simulator')
			flatten()
			buildSelector
			{
				buildNumber('${iOS_Framework_Simulator_arm64_Build}')
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
