package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build

class Appcast extends Build
{
	String name = 'Appcast'
	String label= 'Common'
	String artifacts = 'build/*.msi,build/*.dmg,build/*.tar.gz,build/*.sha256,build/Appcast.xml,build/ReleaseNotes.html,build/docs/**/*.pdf,build/docs/**/*.tar.xz,source/resources/default-*'
	String trigger = null
	List oldBuilds = [-1, 5]
	boolean sendMail = false

	String getSourceJobName(String artifactJob)
	{
		def prefixSource = ''

		if(getTagName())
			prefixSource = 'Release_'

		return buildName(prefixSource, artifactJob)
	}

	Job generate(DslFactory dslFactory)
	{
		def j = super.generate(dslFactory)

		j.with
		{
			if(!getTagName())
			{
				triggers
				{
					upstream(getSourceJobName('Win32_GNU_MSI') + ',' + getSourceJobName('MacOS_DMG') + ',' + getSourceJobName('Source') + ',' + getSourceJobName('Docs'))
				}
			}

			steps
			{
				copyArtifacts(getSourceJobName('Win32_GNU_MSI'))
				{
					buildSelector
					{
						latestSuccessful(true)
					}
				}

				copyArtifacts(getSourceJobName('MacOS_DMG'))
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

				def appCastJob = getTagName() ? '' : '-DJENKINS_APPCAST=\${JOB_NAME}'
				shell("cd build; cmake ../source -DCMAKE_BUILD_TYPE=release -Dtools.only=true ${appCastJob}")
				shell('cd build; cmake -E copy docs/notes/singlehtml/de/appcast.html ReleaseNotes.html')
			}
		}

		return j
	}
}
