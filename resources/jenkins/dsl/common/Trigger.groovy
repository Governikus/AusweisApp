package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build
import common.Constants

class Trigger extends Build
{
	String label = 'Trigger'
	String trigger = null
	List oldBuilds = [3, -1]
	boolean sendMail = false
	boolean disableChangelog = true
	int timeout = 360

	Job generate(DslFactory dslFactory)
	{
		def j = super.generate(dslFactory)

		j.with
		{
			parameters
			{
				fileParam('patch.diff', 'Patch file that will be applied and tested')
				stringParam('REVIEWBOARD_SERVER', Constants.ReviewBoardServer, 'Server')
				stringParam('REVIEWBOARD_REVIEW_ID', '', 'ReviewID')
				stringParam('REVIEWBOARD_DIFF_REVISION', '', 'ReviewDiffRev')
				stringParam('REVIEWBOARD_STATUS_UPDATE_ID', '', 'StatusUpdateID')
				stringParam('Libraries', dslFactory.MERCURIAL_REVISION_BRANCH, 'Copy library artifacts from this job (prefix)')
			}

			steps
			{
				buildDescription('', '${REVIEWBOARD_REVIEW_ID} / ${REVIEWBOARD_DIFF_REVISION}')
			}
		}

		return j
	}
}
