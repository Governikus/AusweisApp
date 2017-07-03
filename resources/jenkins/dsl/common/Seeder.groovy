package common

import javaposse.jobdsl.dsl.Job
import javaposse.jobdsl.dsl.DslFactory

class Seeder
{
	String branch
	String repository

	Job generate(DslFactory dslFactory)
	{
		dslFactory.freeStyleJob('Seeder_' + branch)
		{
			label('Seeder')

			scm
			{
				hg(repository)
				{
					branch(this.branch)
					modules('resources/jenkins/dsl/')
				}
			}

			wrappers
			{
				environmentVariables
				{
					env('MERCURIAL_REVISION_BRANCH', branch)
				}
			}

			steps
			{
				dsl
				{
					external('''\
						resources/jenkins/dsl/Builds/*.groovy
						resources/jenkins/dsl/Libraries/*.groovy
						resources/jenkins/dsl/Views/*.groovy
						resources/jenkins/dsl/Reviews/*.groovy
					'''.stripIndent().trim())

					removeAction('DISABLE')
					removeViewAction('DELETE')
					additionalClasspath('resources/jenkins/dsl')
				}
			}
		}
	}
}
