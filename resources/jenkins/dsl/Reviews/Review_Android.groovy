import common.Review
import common.Constants

// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPKReview)
{

def j = new Review
	(
		name: 'Android_APK_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/**,build/src/libAusweisApp*'
	).generate(this)


j.with
{
	steps
	{
		shell("cmake -P source/ci.cmake -- -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")
	}

	publishers {
		recordIssues {
			tools {
				androidLintParser {
					pattern('**/lint-results*.xml')
				}
			}
			qualityGates {
				warningsQualityGate {
					threshold(1)
					type('TOTAL')
					criticality('UNSTABLE')
				}
			}
		}
	}
}

}


// ----------------------------------------------------------------- AAR
for(ARCH in Constants.AndroidArchAARReview)
{

def j = new Review
	(
		name: 'Android_AAR_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/**,build/debug.symbols/*'
	).generate(this)

j.with
{
	steps
	{
		shell("cmake -P source/ci.cmake -- -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")
	}

	publishers {
		recordIssues {
			tools {
				androidLintParser {
					pattern('**/lint-results*.xml')
				}
			}
			qualityGates {
				warningsQualityGate {
					threshold(1)
					type('TOTAL')
					criticality('UNSTABLE')
				}
			}
		}
	}
}

}


def build = new Review
	(
		name: 'Android_AAR',
		label: 'Common',
		artifacts: 'build/dist/**'
	)

def j = build.generate(this)

j.with
{
	parameters
	{
		for(ARCH in Constants.AndroidArchAARReview)
		{
			stringParam(build.getSourceJobNameParam('Android_AAR_' + ARCH), '', 'Build of ' + ARCH)
		}
	}

	steps
	{
		for(ARCH in Constants.AndroidArchAARReview)
		{
			copyArtifacts(build.getSourceJobName('Android_AAR_' + ARCH))
			{
				flatten()
				buildSelector
				{
					buildNumber('${' + build.getSourceJobNameParam('Android_AAR_' + ARCH) + '}')
				}
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
