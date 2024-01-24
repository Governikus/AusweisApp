import common.Build
import common.Constants

// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPK)
{

def j = new Build
	(
		name: 'Android_APK_' + ARCH,
		libraries: ['Android_' + ARCH],
		label: 'Android',
		artifacts: 'build/dist/**/AusweisApp-*.apk*,build/debug.symbols/*'
	).generate(this)


j.with
{
	steps
	{
		shell("cd source; cmake --preset ci-android-apk -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")
		shell('cmake --build build')
		shell('cmake --install build')
		shell('cmake --build build --target apk')
		shell('cmake --build build --target verify.signature')
		shell('cmake --build build --target dump.apk')
	}

	publishers {
		recordIssues {
			tools {
				androidLintParser {
					pattern('**/lint-results*.xml')
				}
			}
			qualityGates {
				qualityGate {
					threshold(1)
					type('TOTAL')
					unstable(false)
				}
			}
		}
	}
}

}


// ----------------------------------------------------------------- AAR
def neededLibraries = []
for(ARCH in Constants.AndroidArchAAR)
{
	neededLibraries.add('Android_' + ARCH)
}

def j = new Build
	(
		name: 'Android_AAR',
		libraries: neededLibraries,
		label: 'Android',
		artifacts: 'build/dist/**/ausweisapp-*.aar,build/dist/**/ausweisapp-*.pom,build/dist/**/ausweisapp-*.jar,build/**/debug.symbols/*'
	).generate(this)

j.with
{
	steps
	{
		shell('cd source; cmake --preset ci-android-aar')
		shell('cmake --build build')
		shell('cmake --install build')
		shell('cmake --build build --target aar')
		shell("cd build/dist; cmake -DCMD=DEPLOY_NEXUS -P \$WORKSPACE/source/cmake/cmd.cmake")
	}

	publishers {
		recordIssues {
			tools {
				androidLintParser {
					pattern('**/lint-results*.xml')
				}
			}
			qualityGates {
				qualityGate {
					threshold(1)
					type('TOTAL')
					unstable(false)
				}
			}
		}
	}
}
