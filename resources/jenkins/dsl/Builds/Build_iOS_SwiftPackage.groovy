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
			targetDirectory('build/arm64')
			buildSelector
			{
				latestSuccessful(true)
			}
		}

		copyArtifacts(build.getSourceJobName('iOS_Simulator_Framework'))
		{
			targetDirectory('build/x86_64-simulator')
			buildSelector
			{
				latestSuccessful(true)
			}
		}

		shell('cd build; cmake -P ../source/cmake/SwiftPackage.cmake')
	}
}
