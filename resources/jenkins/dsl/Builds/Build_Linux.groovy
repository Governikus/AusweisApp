import common.Build

def j = new Build
	(
		name: 'Linux',
		libraries: 'Linux',
		label: 'Linux',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}

	publishers
	{
		recordCoverage
		{
			ignoreParsingErrors(true)

			sourceDirectories
			{
				sourceCodeDirectory
				{
					path('source')
				}
			}

			tools
			{
				coverageTool
				{
					parser('COBERTURA')
					pattern('build/gcovr.xml')
				}
			}
		}

		slocCount
		{
			pattern('build/cloc.xml')
			encoding('UTF-8')
			commentIsCode(false)
			buildsInGraph(0)
			ignoreBuildFailure(false)
		}
	}
}
