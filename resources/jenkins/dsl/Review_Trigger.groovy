import common.RootTrigger

def j = new RootTrigger
	(
		jobName: '_Review_Trigger_',
		triggerJob: '${REVIEWBOARD_REVIEW_BRANCH}__Review_Trigger'
	).generate(this)
