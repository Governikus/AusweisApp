package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build

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
				stringParam('Libraries', dslFactory.MERCURIAL_REVISION_BRANCH, 'Copy library artifacts from this job (prefix)')
				stringParam('review.url', '', 'Review URL that will be used for description (optional)')
			}

			steps
			{
				buildDescription('', '${review.url}')
			}
		}

		return j
	}
}
