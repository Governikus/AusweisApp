package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

class Build
{
	enum JobType
	{
		Freestyle, Multi, Matrix
	}

	JobType jobType = JobType.Freestyle
	String name
	String namePrefix = ''
	String libraries
	String artifacts
	String label
	String trigger = '@daily'
	String excludePattern = 'source/**'
	List oldBuilds = [7, 10]
	boolean releaseJob = false
	boolean xunit = false
	boolean sendMail = true
	boolean disableChangelog = false
	boolean allowEmptyArtifacts = false
	boolean review = false
	int timeout = 60
	int weight = -1

	DslFactory dslFactory


	String buildName(String prefix, String name, boolean libs=false)
	{
		def branchPrefix = ''

		if(!getReleaseJob())
		{
			if(libs && getReview())
				branchPrefix = '${Libraries}_'
			else
				branchPrefix = dslFactory.MERCURIAL_REVISION_BRANCH + '_'
		}

		return branchPrefix + prefix + name
	}

	String getJobName()
	{
		return getSourceJobName(getName())
	}

	String getSourceJobName(String artifactJob)
	{
		return buildName(getNamePrefix(), artifactJob)
	}

	String getLibName(String partialLibName)
	{
		def prefix = 'Libs_'

		if(getReleaseJob())
		{
			prefix = '${MERCURIAL_REVISION_BRANCH}_' + prefix
		}

		return buildName(prefix, partialLibName, true)
	}

	boolean cleanUpWs()
	{
		return true
	}



	Job generate(DslFactory factory)
	{
		dslFactory = factory
		def j


		if(getJobType() == JobType.Multi)
			j = dslFactory.multiJob(getJobName())
		else if(getJobType() == JobType.Matrix)
			j = dslFactory.matrixJob(getJobName())
		else if(getJobType() == JobType.Freestyle)
			j = dslFactory.freeStyleJob(getJobName())
		else
			throw new Exception('Unknown job type: ' + getJobType())


		j.with
		{
			if(getLabel())
				label(getLabel())

			if(getOldBuilds())
				logRotator(*getOldBuilds())

			if(getWeight() > 1)
				weight(getWeight())

			String defaultArtifactJob = 'LIBRARY_ARTIFACT_JOB'
			String defaultArtifactJobEval = 'LIBRARY_ARTIFACT_JOB_EVAL'
			String defaultArtifactParam = 'LIBRARY_ARTIFACT'

			environmentVariables
			{
				env('TEMP', '$WORKSPACE/tmp')
				env('TMP', '$WORKSPACE/tmp')
				env('TMPDIR', '$WORKSPACE/tmp')
				env(defaultArtifactJobEval, '$' + defaultArtifactJob)
			}

			if(getLibraries())
			{
				parameters
				{
					stringParam(defaultArtifactJob, getLibName(getLibraries()), "Use job artifact source")
					buildSelectorParam(defaultArtifactParam)
					{
						defaultBuildSelector
						{
							latestSuccessful(true)
						}
						description('Use build library job')
					}
				}
			}

			wrappers
			{
				preBuildCleanup()
				{
					if(!getReleaseJob() && getExcludePattern())
						for(String value in getExcludePattern().split(','))
							excludePattern(value)

					deleteDirectories(true)
				}

				timestamps()

				if(getTimeout() > -1)
				{
					timeout
					{
						absolute(getTimeout())
					}
				}
			}

			scm
			{
				hg(dslFactory.MERCURIAL_REPOSITORY_URL)
				{
					if(getReleaseJob())
						tag('${changeset}')
					else
						tag(dslFactory.MERCURIAL_REVISION_BRANCH)

					subdirectory('source')
					if(getDisableChangelog())
						disableChangeLog()
					clean()
				}
			}

			if(getTrigger())
			{
				triggers
				{
					cron('@weekly')
					scm(getTrigger())
				}
			}

			steps
			{
				if(getLibraries())
				{
					String targetDestDir = 'libs'
					copyArtifacts('${' + defaultArtifactJobEval + '}')
					{
						targetDirectory(targetDestDir)
						buildSelector
						{
							includePatterns('**/Toolchain_*')
							flatten()
							buildParameter(defaultArtifactParam)
						}
					}

					if(getName().contains('_Win'))
						batchFile("cd ${targetDestDir} & FOR %%a in (Toolchain_*) DO cmake -E tar xf %%a")
					else
						shell("cd ${targetDestDir}; cmake -E tar xf Toolchain_*")
				}

				if(getName().contains('_Win'))
					batchFile('mkdir build & mkdir tmp')
				else
					shell('mkdir build; mkdir -m 700 tmp')
			}

			publishers
			{
				if(getXunit())
				{
					archiveXUnit
					{
						qTestLib
						{
							pattern('build/**/results.*.log.xml')
						}

						failedThresholds
						{
							unstable(0)
							unstableNew(0)
							failure(100)
							failureNew(100)
						}

						skippedThresholds
						{
							unstable(100)
							unstableNew(100)
							failure(100)
							failureNew(100)
						}

						thresholdMode(ThresholdMode.PERCENT)
					}
				}

				if(getArtifacts())
				{
					archiveArtifacts()
					{
						pattern(getArtifacts())
						allowEmpty(getAllowEmptyArtifacts())
						fingerprint()
					}
				}

				wsCleanup()
				{
					cleanWhenUnstable(cleanUpWs())
					cleanWhenFailure(cleanUpWs())
					cleanWhenNotBuilt(cleanUpWs())
					cleanWhenAborted(cleanUpWs())
					for(String value in getExcludePattern().split(','))
						excludePattern(value)
					deleteDirectories(true)
				}

				if(getSendMail())
					mailer('', true, true)
			}
		}

		return j
	}
}
