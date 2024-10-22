import common.Release

build = new Release
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
		buildSelectorParam('iOS_Framework_Build')
		{
			defaultBuildSelector
			{
				latestSuccessful(true)
			}
			description('Build to get iOS Framework artifacts from')
		}

		buildSelectorParam('iOS_Simulator_Framework_Build')
		{
			defaultBuildSelector
			{
				latestSuccessful(true)
			}
			description('Build to get iOS Simulator Framework artifacts from')
		}
		buildSelectorParam('iOS_Simulator_arm64_Framework_Build')
		{
			defaultBuildSelector
			{
				latestSuccessful(true)
			}
			description('Build to get iOS Simulator arm64 Framework artifacts from')
		}
	}

	steps
	{
		copyArtifacts(build.getSourceJobName('iOS_Framework'))
		{
			targetDirectory('arm64')
			flatten()
			buildSelector
			{
				buildParameter('iOS_Framework_Build')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_Framework'))
		{
			targetDirectory('x86_64-simulator')
			flatten()
			buildSelector
			{
				buildParameter('iOS_Simulator_Framework_Build')
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_arm64_Framework'))
		{
			targetDirectory('arm64-simulator')
			flatten()
			buildSelector
			{
				buildParameter('iOS_Simulator_arm64_Framework_Build')
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
