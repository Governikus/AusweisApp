import common.RootTrigger

def j = new RootTrigger
	(
		jobName: '_Review_Trigger_',
		triggerJob: '${REVIEW_BRANCH}__Review_Trigger'
	).generate(this)


job('_Review_Check_')
{
	logRotator(3, 50)
	label('Trigger')

	triggers
	{
		cron('H/10 * * * *')
	}

	configure
	{
		project -> project / 'builders' << 'org.jenkinsci.plugins.jenkinsreviewbot.ReviewboardPollingBuilder' {
			reviewbotJobName '_Review_Trigger_'
			checkBackPeriod '2'
			reviewbotRepoId '1'
			restrictByUser 'true'
			disableAdvanceNotice 'true'
			processedReviewDates ''
		}
	}
}
