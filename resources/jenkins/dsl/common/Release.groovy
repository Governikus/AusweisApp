package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build

class Release extends Build
{
	String namePrefix = 'Release_'
	String trigger = null
	List oldBuilds = [365, -1]
	boolean releaseJob = true
	boolean sendMail = false

	Job generate(DslFactory dslFactory)
	{
		def j = super.generate(dslFactory)

		j.with
		{
			parameters
			{
				stringParam('changeset', 'release', 'Build given changeset (tag) as release')
			}

			wrappers
			{
				buildName('${changeset}')
			}
		}

		return j
	}
}
