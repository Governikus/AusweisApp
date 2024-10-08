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
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell("cd source; cmake --preset ci-android-apk-review -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")
		shell('cmake --build build')
		shell('cmake --build build --target apk')
		shell('cmake --build build --target verify.signature')
		shell('cmake --build build --target dump.apk')
		shell('ctest --test-dir build --output-on-failure')
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

def j = new Review
	(
		name: 'Android_AAR',
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/**,build/debug.symbols/*'
	).generate(this)

j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')
		shell('cd source; cmake --preset ci-android-aar-review')
		shell('cmake --build build')
		shell('cmake --build build --target aar')
		shell('ctest --test-dir build --output-on-failure')
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
