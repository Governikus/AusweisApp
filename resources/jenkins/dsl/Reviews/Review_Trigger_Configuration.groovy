import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createEnvMap
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

def envMap = createEnvMap(getJobs(), this.&getName)
def reviewMessage = createReviewMessage(getJobs(), this.&getName)


def j = new Trigger
	(
		name: '_Review_Trigger_Configuration',
		jobType: JobType.Multi
	).generate(this)


j.with
{
	concurrentBuild()

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

		phase('General', 'UNSTABLE')
		{
			phaseJob(getName('Formatting'))
		}

		phase('Unit Tests', 'UNSTABLE')
		{
			phaseJob(getName('Configuration'))
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
