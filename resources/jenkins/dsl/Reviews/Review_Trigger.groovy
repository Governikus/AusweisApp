import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createEnvMap
import static common.Constants.createReviewMessage

def getJobs()
{
	def list = ['Formatting', 'Source', 'Docs', 'MacOS_DMG', 'Win32_GNU_MSI', 'Win32_MSVC_MSI', 'iOS_IPA']
	for(ARCH in Constants.AndroidArch)
	{
		list << 'Android_APK_' + ARCH
	}

	def unitTests = ['Linux', 'MacOS', 'Win32_GNU']
	list += unitTests

	return list
}

String getName(String name)
{
	return "${MERCURIAL_REVISION_BRANCH}_Review_" + name
}

def envMap = createEnvMap(getJobs(), this.&getName)
def reviewMessage = createReviewMessage(getJobs(), this.&getName) + ' | Libraries: ${Libraries}'



def j = new Trigger
	(
		name: '_Review_Trigger',
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

			phaseJob(getName('Source'))

			phaseJob(getName('Docs'))
		}

		phase('Packages')
		{
			for(ARCH in Constants.AndroidArch)
			{
				phaseJob(getName('Android_APK_' + ARCH))
			}

			phaseJob(getName('Win32_GNU_MSI'))

			phaseJob(getName('Win32_MSVC_MSI'))

			phaseJob(getName('MacOS_DMG'))

			phaseJob(getName('iOS_IPA'))
		}

		phase('Unit Tests')
		{
			phaseJob(getName('Linux'))

			phaseJob(getName('Win32_GNU'))

			phaseJob(getName('MacOS'))
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
