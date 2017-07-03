categorizedJobsView("${MERCURIAL_REVISION_BRANCH}")
{
	jobs
	{
		regex("${MERCURIAL_REVISION_BRANCH}_(.)*")
	}

	categorizationCriteria
	{
		regexGroupingRule('^[a-z]*_Libs_', '_Libraries_')
		regexGroupingRule('_Review_', '_Reviews_')
	}

	columns
	{
		status()
		weather()
		categorizedJob()
		lastSuccess()
		lastFailure()
		lastDuration()
		lastBuildConsole()
		buildButton()
	}
}
