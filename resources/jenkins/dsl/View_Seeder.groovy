listView('Seeder')
{
	jobs
	{
		regex('(.)*Seeder(.)*')
	}

	columns
	{
		status()
		weather()
		name()
		lastSuccess()
		lastFailure()
		lastBuildConsole()
		buildButton()
	}
}
