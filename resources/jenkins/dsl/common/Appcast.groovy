package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build

class Appcast extends Build
{
	String name = 'Appcast'
	String label= 'Common'
	String artifacts = 'build/*.msi,build/*.dmg,build/*.tar.gz,build/*.sha256,build/Appcast*,build/ReleaseNotes.html,build/*.pdf,build/docs/**/*.pdf,build/docs/**/*.tar.xz'
	String trigger = null
	List oldBuilds = [-1, 5]
	boolean sendMail = false

	Job generate(DslFactory dslFactory)
	{
		def j = super.generate(dslFactory)

		j.with
		{
			if(!getReleaseJob())
			{
				triggers
				{
					upstream(getSourceJobName('Win64_MSVC_MSI') + ',' + getSourceJobName('MacOS_DMG_PKG') + ',' + getSourceJobName('Source') + ',' + getSourceJobName('Docs'))
				}
			}

			steps
			{
				copyArtifacts(getSourceJobName('Win64_MSVC_MSI'))
				{
					buildSelector
					{
						latestSuccessful(true)
					}
				}

				copyArtifacts(getSourceJobName('MacOS_DMG_PKG'))
				{
					buildSelector
					{
						latestSuccessful(true)
					}
				}

				copyArtifacts(getSourceJobName('Source'))
				{
					buildSelector
					{
						latestSuccessful(true)
					}
				}


				copyArtifacts(getSourceJobName('Docs'))
				{
					buildSelector
					{
						latestSuccessful(true)
					}
				}

				shell("cd source; cmake --preset ci-tools")
				shell('cd build; cmake -E copy docs/notes/singlehtml/de/appcast.html ReleaseNotes.html')
			}
		}

		return j
	}
}
