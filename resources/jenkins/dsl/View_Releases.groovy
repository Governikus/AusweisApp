listView('Releases')
{
	jobs
	{
		regex('Release(.)*')
	}

	columns
	{
		status()
		name()
		lastSuccess()
		lastBuildConsole()
		buildButton()
	}
}
