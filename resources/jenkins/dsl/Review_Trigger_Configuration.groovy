import common.RootTrigger

def j = new RootTrigger
	(
		jobName: '_Review_Trigger_Configuration_',
		triggerJob: '${REVIEW_BRANCH}__Review_Trigger_Configuration'
	).generate(this)
