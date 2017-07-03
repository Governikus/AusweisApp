package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

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
