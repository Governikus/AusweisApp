import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createEnvMap
import static common.Constants.createReviewMessage
import static common.Constants.getEnvNumber

def getJobs()
{
	def list = ['Formatting', 'Source', 'Docs']

	def packages = ['MacOS_DMG_PKG', 'Win32_GNU_MSI', 'Win32_MSVC_MSI', 'iOS_IPA', 'iOS_Framework', 'iOS_Simulator_Framework', 'Android_AAR']
	for(ARCH in Constants.AndroidArchAPKReview)
	{
		packages << 'Android_APK_' + ARCH
	}
	list += packages

	def subPackages = ['iOS_SwiftPackage']
	list += subPackages

	def unitTests = ['Linux', 'Linux_Integrated', 'MacOS', 'MacOS_Integrated', 'Win32_GNU', 'FreeBSD']
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

		phase('Packages', 'UNSTABLE')
		{
			phaseJob(getName('Android_AAR'))

			for(ARCH in Constants.AndroidArchAPKReview)
			{
				phaseJob(getName('Android_APK_' + ARCH))
			}

			phaseJob(getName('Win32_GNU_MSI'))

			phaseJob(getName('Win32_MSVC_MSI'))

			phaseJob(getName('MacOS_DMG_PKG'))

			phaseJob(getName('iOS_IPA'))

			phaseJob(getName('iOS_Framework'))

			phaseJob(getName('iOS_Simulator_Framework'))
		}

		phase('Sub-Packages', 'UNSTABLE')
		{
			phaseJob(getName('iOS_SwiftPackage'))
			{
				parameters
				{
					predefinedProp('iOS_Framework_Build', getEnvNumber(getName('iOS_Framework')))
					predefinedProp('iOS_Simulator_Framework_Build', getEnvNumber(getName('iOS_Simulator_Framework')))
				}
			}
		}

		phase('Unit Tests')
		{
			phaseJob(getName('Linux'))

			phaseJob(getName('Linux_Integrated'))

			phaseJob(getName('Win32_GNU'))

			phaseJob(getName('MacOS'))

			phaseJob(getName('MacOS_Integrated'))

			phaseJob(getName('FreeBSD'))
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
