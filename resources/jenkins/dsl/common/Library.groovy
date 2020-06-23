package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Build

class Library extends Build
{
	String artifacts = 'build/Toolchain_*'
	String namePrefix = 'Libs_' // See copyArtifacts in Build.groovy
	String trigger = null
	List oldBuilds = [-1, 5]
	int timeout = -1
	int weight = 2
}
