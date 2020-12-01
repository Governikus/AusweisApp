package common

class Constants
{
	static final AndroidArchAPKReview = ["armeabi-v7a", "arm64-v8a"]
	static final AndroidArchAPK = AndroidArchAPKReview + ["x86"]
	static final AndroidArchAAR = ["arm64-v8a"]
	static final AndroidArch = (AndroidArchAPK + AndroidArchAAR).unique()

	static String strip(String content)
	{
		content = content.stripIndent().trim()
		return content.replaceAll("\n", " ");
	}




	// Helper functions for Multijob-Trigger!

	static String escapeEnv(String name)
	{
		return name.replaceAll('-', '_').replaceAll('\\.', '_').toUpperCase()
	}

	static String getEnvVariable(String jobName, String suffix)
	{
		return '${' + escapeEnv(jobName) + '_' + suffix + '}'
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
		return name + ': [' + getEnvResult(jobName) + '](${JENKINS_URL}job/' + jobName + '/' + getEnvNumber(jobName) + '/)'
	}

	static Map createEnvMap(jobs, nameGenerator)
	{
		def map = [:]

		jobs.each
		{
			String name = nameGenerator("${it}")
			name = escapeEnv(name)
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
