package common

class Constants
{
	static final AndroidArchAPK = ["armeabi-v7a", "x86", "arm64-v8a"]
	static final AndroidArchAAR = ["arm64-v8a"]
	static final AndroidArch = (AndroidArchAPK + AndroidArchAAR).unique()

	static String strip(String content)
	{
		content = content.stripIndent().trim()
		return content.replaceAll("\n", " ");
	}




	// Helper functions for Multijob-Trigger!

	static String getEnvVariable(String jobName, String suffix)
	{
		return '${' + jobName.toUpperCase() + '_' + suffix + '}'
	}

	static String getEnvResult(String name)
	{
		return getEnvVariable(name, 'BUILD_RESULT')
	}

	static String getEnvNumber(String name)
	{
		return getEnvVariable(name, 'BUILD_NUMBER')
	}

	static String genMsgName(String name, nameGenerator)
	{
		def jobName = nameGenerator(name)
		def envJobName = jobName.replaceAll('-', '_').replaceAll('\\.', '_').toUpperCase()

		return name + ': [' + getEnvResult(envJobName) + '](${JENKINS_URL}job/' + jobName + '/' + getEnvNumber(envJobName) + '/)'
	}

	static Map createEnvMap(jobs, nameGenerator)
	{
		def map = [:]

		jobs.each
		{
			String name = nameGenerator("${it}")
			name = name.replaceAll('-', '_').replaceAll('\\.', '_').toUpperCase()
			map << [(name + '_BUILD_RESULT'):'']
			map << [(name + '_BUILD_NUMBER'):'']
		}

		return map
	}

	static String createReviewMessage(jobs, nameGenerator)
	{
		def list = []

		jobs.each
		{
			list << genMsgName("${it}", nameGenerator)
		}

		return list.join(' | ')
	}
}
