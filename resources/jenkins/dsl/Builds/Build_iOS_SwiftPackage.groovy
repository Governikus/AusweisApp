import common.Build

build = new Build
	(
		name: 'iOS_SwiftPackage',
		label: 'iOS',
		artifacts: 'build/dist/*.zip'
	)

def j = build.generate(this)


j.with
{
	steps
	{
		copyArtifacts(build.getSourceJobName('iOS_Framework'))
		{
			targetDirectory('arm64')
			flatten()
			buildSelector
			{
				latestSuccessful(true)
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_Framework'))
		{
			targetDirectory('x86_64-simulator')
			flatten()
			buildSelector
			{
				latestSuccessful(true)
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_arm64_Framework'))
		{
			targetDirectory('arm64-simulator')
			flatten()
			buildSelector
			{
				latestSuccessful(true)
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
