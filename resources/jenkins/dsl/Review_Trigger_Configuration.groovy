import common.RootTrigger

def j = new RootTrigger
	(
		jobName: '_Review_Trigger_Configuration_',
		triggerJob: '${REVIEWBOARD_REVIEW_BRANCH}__Review_Trigger_Configuration'
	).generate(this)
