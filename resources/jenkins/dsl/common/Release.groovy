package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build

class Release extends Build
{
	String namePrefix = 'Release_'
	String tagName = 'release'
	String trigger = null
	List oldBuilds = null
	boolean cleanup = true
	boolean sendMail = false

	Job generate(DslFactory dslFactory)
	{
		def j = super.generate(dslFactory)

		j.with
		{

		}

		return j
	}
}
