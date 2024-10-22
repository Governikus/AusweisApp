import common.Trigger
import common.Constants
import common.Build.JobType
import static common.Constants.createReviewMessage
import static common.Constants.getEnvNumber

def getJobs()
{
	def list = ['Formatting', 'Source', 'Docs']

	def packages = ['Container', 'MacOS_DMG_PKG', 'Win64_GNU_MSI', 'Win64_MSVC_MSI', 'iOS_IPA', 'iOS_Framework', 'iOS_Simulator_Framework', 'iOS_Simulator_arm64_Framework']
	for(ARCH in Constants.AndroidArchAARReview)
	{
		packages << 'Android_AAR_' + ARCH
	}
	for(ARCH in Constants.AndroidArchAPKReview)
	{
		packages << 'Android_APK_' + ARCH
	}
	list += packages

	def subPackages = ['Android_AAR', 'iOS_SwiftPackage']
	list += subPackages

	def unitTests = ['Linux', 'Linux_Integrated', 'MacOS', 'MacOS_Integrated', 'Win64_GNU', 'Win64_MSVC', 'FreeBSD', 'SonarQube', 'Docker_VNC']
	list += unitTests

	return list
}

String getName(String name)
{
	return "${MERCURIAL_REVISION_BRANCH}_Review_" + name
}

String getNameParam(String name)
{
	return getName(name).replace('-', '_') + '_Build'
}

def reviewMessage = createReviewMessage(getJobs(), this.&getName) + ' | Libraries: ${Libraries}' + ' | SonarQube Analysis: ${SONARQUBE_ANALYSIS_MESSAGE}'



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
			env('SONARQUBE_ANALYSIS_MESSAGE', '')
		}
	}

	steps
	{
		shell('cd source; resources/jenkins/notify_rb.sh')

		shell('cd source; hg --config patch.eol=auto import --no-commit ../patch.diff')

		phase('General', 'UNSTABLE')
		{
			phaseJob(getName('Formatting'))

			phaseJob(getName('Source'))

			phaseJob(getName('Docs'))
		}

		phase('Packages', 'UNSTABLE')
		{
			for(ARCH in Constants.AndroidArchAARReview)
			{
				phaseJob(getName('Android_AAR_' + ARCH))
			}

			for(ARCH in Constants.AndroidArchAPKReview)
			{
				phaseJob(getName('Android_APK_' + ARCH))
			}

			phaseJob(getName('Win64_GNU_MSI'))

			phaseJob(getName('Win64_MSVC_MSI'))

			phaseJob(getName('MacOS_DMG_PKG'))

			phaseJob(getName('iOS_IPA'))

			phaseJob(getName('iOS_Framework'))

			phaseJob(getName('iOS_Simulator_Framework'))

			phaseJob(getName('iOS_Simulator_arm64_Framework'))

			phaseJob(getName('Container'))
		}

		phase('Sub-Packages', 'UNSTABLE')
		{
			phaseJob(getName('Android_AAR'))
			{
				parameters
				{
					for(ARCH in Constants.AndroidArchAARReview)
					{
						predefinedProp(getNameParam('Android_AAR_' + ARCH), getEnvNumber(getName('Android_AAR_' + ARCH)))
					}
				}
			}

			phaseJob(getName('iOS_SwiftPackage'))
			{
				parameters
				{
					predefinedProp('iOS_Framework_Build', getEnvNumber(getName('iOS_Framework')))
					predefinedProp('iOS_Simulator_Framework_Build', getEnvNumber(getName('iOS_Simulator_Framework')))
					predefinedProp('iOS_Simulator_arm64_Framework_Build', getEnvNumber(getName('iOS_Simulator_arm64_Framework')))
				}
			}
		}

		phase('Unit Tests')
		{
			phaseJob(getName('Linux'))

			phaseJob(getName('Linux_Integrated'))

			phaseJob(getName('Win64_GNU'))

			phaseJob(getName('Win64_MSVC'))

			phaseJob(getName('MacOS'))

			phaseJob(getName('MacOS_Integrated'))

			phaseJob(getName('FreeBSD'))

			phaseJob(getName('SonarQube'))

			phaseJob(getName('Docker_VNC'))
		}

		copyArtifacts(getName('SonarQube'))
		{
			includePatterns('*/sonar-metadata.txt')
			flatten()
			buildSelector
			{
				multiJobBuild()
			}
		}

		shell('''\
		if [ -f sonar-metadata.txt ]; then
			echo "sonar-metadata.txt exists";
			URL=$(grep dashboardUrl sonar-metadata.txt | cut -d"=" -f 2-);
			echo -n "SONARQUBE_ANALYSIS_MESSAGE=" > sonar_analysis_message.env;
			echo -n "[Result](" >> sonar_analysis_message.env;
			echo -n $URL >> sonar_analysis_message.env; echo -n ")" >> sonar_analysis_message.env;
		else echo "sonar-metadata.txt does not exist";
			echo -n "SONARQUBE_ANALYSIS_MESSAGE=No sonar analysis available" > sonar_analysis_message.env;
		fi
		''')

		environmentVariables
		{
			propertiesFile('sonar_analysis_message.env')
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
