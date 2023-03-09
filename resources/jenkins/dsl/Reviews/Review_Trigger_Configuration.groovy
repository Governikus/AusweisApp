import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createReviewMessage
import static common.Constants.getEnvNumber

def getJobs()
{
	return ['Formatting', 'Configuration']
}

String getName(String name)
{
	return "${MERCURIAL_REVISION_BRANCH}_Review_" + name
}

def reviewMessage = createReviewMessage(getJobs(), this.&getName)


def j = new Trigger
	(
		name: '_Review_Trigger_Configuration',
		jobType: JobType.Multi
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cd source; resources/jenkins/notify_rb.sh')

		shell('cd source; hg import --no-commit ../patch.diff')

		phase('General', 'UNSTABLE')
		{
			phaseJob(getName('Formatting'))
		}

		phase('Unit Tests', 'UNSTABLE')
		{
			phaseJob(getName('Configuration'))
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
