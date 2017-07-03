import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createEnvMap
import static common.Constants.createReviewMessage

def getJobs()
{
	def list = ['Linux', 'MacOS', 'Win32_GNU', 'Win32_MSVC', 'iOS']
	for(ARCH in Constants.AndroidArch)
	{
		list << 'Android_' + ARCH
	}

	list << "${MERCURIAL_REVISION_BRANCH}__Review_Trigger"

	return list
}

String getName(String name)
{
	if(name.endsWith('Review_Trigger'))
	{
		return name
	}

	return "${MERCURIAL_REVISION_BRANCH}_Review_Libs_" + name
}

def envMap = createEnvMap(getJobs(), this.&getName)
def reviewMessage = createReviewMessage(getJobs(), this.&getName)



def j = new Trigger
	(
		name: '_Review_Trigger_Libs',
		jobType: JobType.Multi,
		timeout: 720
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			envs(envMap)
		}
	}

	steps
	{
		shell('cd source; hg import --no-commit ../patch.diff')

		phase('Libraries')
		{
			phaseJob(getName('Linux'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			for(ARCH in Constants.AndroidArch)
			{
				phaseJob(getName('Android_' + ARCH))
				{
					abortAllJobs(false)
					killPhaseCondition('NEVER')
				}
			}

			phaseJob(getName('Win32_GNU'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			phaseJob(getName('Win32_MSVC'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			phaseJob(getName('MacOS'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			phaseJob(getName('iOS'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}
		}

		phase('Application')
		{
			phaseJob("${MERCURIAL_REVISION_BRANCH}__Review_Trigger")
			{
				parameters
				{
					currentBuild()
					predefinedProp('Libraries', "${MERCURIAL_REVISION_BRANCH}_Review")
				}
			}
		}
	}

	configure
	{
		project -> project / 'publishers' << 'org.jenkinsci.plugins.jenkinsreviewbot.ReviewboardNotifier' {
			shipItOnSuccess 'false'
			useMarkdown 'true'
			customMessage "${reviewMessage}"
		}
	}
}
