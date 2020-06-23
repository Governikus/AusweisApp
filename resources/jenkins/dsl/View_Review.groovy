categorizedJobsView('Review')
{
	jobs
	{
		regex(/.*_Review_.*/)
	}

	categorizationCriteria
	{
		regexGroupingRule(/^[a-z0-9.]+_Review_/, 'Branches')
	}

	categorizationCriteria
	{
		regexGroupingRule(/Review_Libs/, 'Libraries')
	}

	categorizationCriteria
	{
		regexGroupingRule(/__Review_Trigger/, 'Sub-Trigger')
	}

	categorizationCriteria
	{
		regexGroupingRule(/^[A-Z]\w*_Review_/, 'Tools')
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
