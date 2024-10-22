import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createReviewMessage

def getJobs()
{
	def list = ['Linux', 'MacOS', 'Win64_GNU', 'Win64_MSVC', 'Win64_MSVC_dev', 'iOS', 'iOS_Simulator', 'iOS_Simulator_arm64', 'FreeBSD']
	for(ARCH in Constants.AndroidArchLibsReview)
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

def reviewMessage = createReviewMessage(getJobs(), this.&getName)



def j = new Trigger
	(
		name: '_Review_Trigger_Libs',
		jobType: JobType.Multi,
		timeout: 720
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; resources/jenkins/notify_rb.sh')

		shell('cd source; hg --config patch.eol=auto import --no-commit ../patch.diff')

		phase('Libraries')
		{
			phaseJob(getName('Linux'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			for(ARCH in Constants.AndroidArchLibsReview)
			{
				phaseJob(getName('Android_' + ARCH))
				{
					abortAllJobs(false)
					killPhaseCondition('NEVER')
				}
			}

			phaseJob(getName('Win64_GNU'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			phaseJob(getName('Win64_MSVC'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			phaseJob(getName('Win64_MSVC_dev'))
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

			phaseJob(getName('iOS_Simulator'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}
			phaseJob(getName('iOS_Simulator_arm64'))
			{
				abortAllJobs(false)
				killPhaseCondition('NEVER')
			}

			phaseJob(getName('FreeBSD'))
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

	publishers {
		postBuildScript
		{
			markBuildUnstable(true)
			buildSteps
			{
				postBuildStep
				{
					stopOnFailure(false)
					results(['SUCCESS', 'FAILURE', 'UNSTABLE', 'ABORTED', 'NOT_BUILT'])
					buildSteps
					{
						envInjectBuilder
						{
							propertiesFilePath('')
							propertiesContent("REVIEWBOARD_MSG=" + reviewMessage)
						}

						shell
						{
							command('cd source; resources/jenkins/notify_rb.sh')
						}
					}
				}
			}
		}
	}
}
