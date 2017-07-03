package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

class RootTrigger
{
	String jobName
	String triggerJob

	Job generate(DslFactory dslFactory)
	{
		def j = dslFactory.freeStyleJob(getJobName())

		j.with
		{
			logRotator(14, -1)
			label('Trigger')

			configure
			{
				project -> project / 'properties' / 'hudson.model.ParametersDefinitionProperty' / 'parameterDefinitions' << 'org.jenkinsci.plugins.jenkinsreviewbot.ReviewboardParameterDefinition' {
					name('review.url')
					defaultValue('')
					description('ID of review request')
				}
			}

			steps
			{
				buildDescription('', '${REVIEW_URL}')

				downstreamParameterized
				{
					trigger(getTriggerJob())
					{
						parameters
						{
							predefinedProp('review.url', '${REVIEW_URL}')
						}

						parameterFactories
						{
							forMatchingFiles('patch.diff', 'patch.diff', 'FAIL')
						}
					}
				}
			}
		}

		return j
	}
}
