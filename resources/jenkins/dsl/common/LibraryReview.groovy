package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Constants
import common.Library

class LibraryReview extends Library
{
	String namePrefix = 'Review_Libs_' // See copyArtifacts in Build.groovy
	boolean sendMail = false
	boolean disableChangelog = true

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
			}

			steps
			{
				buildDescription('', '${REVIEWBOARD_REVIEW_ID} / ${REVIEWBOARD_DIFF_REVISION}')
			}
		}

		return j
	}
}
