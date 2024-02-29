package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

import common.Constants

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

			parameters
			{
				stringParam('REVIEWBOARD_SERVER', '', 'Server')
				stringParam('REVIEWBOARD_REVIEW_ID', '', 'ReviewID')
				stringParam('REVIEWBOARD_DIFF_REVISION', '', 'ReviewDiffRev')
				stringParam('REVIEWBOARD_STATUS_UPDATE_ID', '', 'StatusUpdateID')
				stringParam('REVIEWBOARD_REVIEW_BRANCH', '', 'ReviewBranch')
			}

			steps
			{
				buildDescription('', '${REVIEWBOARD_REVIEW_ID} / ${REVIEWBOARD_DIFF_REVISION}')

				shell('rbt status-update set --url ${BUILD_URL} --url-text "See trigger" -r ${REVIEWBOARD_REVIEW_ID} -s ${REVIEWBOARD_STATUS_UPDATE_ID} --server ${REVIEWBOARD_SERVER} --username ${REVIEWBOARD_USER} --api-token ${REVIEWBOARD_TOKEN}')

				shell('rbt patch --write patch.diff --server ${REVIEWBOARD_SERVER} --diff-revision ${REVIEWBOARD_DIFF_REVISION} ${REVIEWBOARD_REVIEW_ID}')

				downstreamParameterized
				{
					trigger(getTriggerJob())
					{
						parameters
						{
							predefinedProp('REVIEWBOARD_SERVER', '${REVIEWBOARD_SERVER}')
							predefinedProp('REVIEWBOARD_REVIEW_ID', '${REVIEWBOARD_REVIEW_ID}')
							predefinedProp('REVIEWBOARD_DIFF_REVISION', '${REVIEWBOARD_DIFF_REVISION}')
							predefinedProp('REVIEWBOARD_STATUS_UPDATE_ID', '${REVIEWBOARD_STATUS_UPDATE_ID}')
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
