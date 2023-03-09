import common.RootTrigger

def j = new RootTrigger
	(
		jobName: '_Review_Trigger_Libs_',
		triggerJob: '${REVIEWBOARD_REVIEW_BRANCH}__Review_Trigger_Libs'
	).generate(this)
