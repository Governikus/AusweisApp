import common.Build
import common.Constants

// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPK)
{

def j = new Build
	(
		name: 'Android_APK_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/**,build/src/libAusweisApp*'
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env('QT_ANDROID_KEYSTORE_PATH', '${APK_SIGN_KEYSTORE_DEV}')
			env('QT_ANDROID_KEYSTORE_ALIAS', '${APK_SIGN_KEYSTORE_ALIAS_DEV}')
			env('QT_ANDROID_KEYSTORE_STORE_PASS', '${APK_SIGN_KEYSTORE_PSW_DEV}')
		}
	}

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
for(ARCH in Constants.AndroidArchAAR)
{

def j = new Build
	(
		name: 'Android_AAR_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/**,build/**/debug.symbols/*'
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


def build = new Build
	(
		name: 'Android_AAR',
		label: 'Android',
		artifacts: 'build/dist/**'
	)

def j = build.generate(this)

j.with
{
	steps
	{
		for(ARCH in Constants.AndroidArchAAR)
		{
			copyArtifacts(build.getSourceJobName('Android_AAR_' + ARCH))
			{
				flatten()
				buildSelector
				{
					latestSuccessful(true)
				}
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
