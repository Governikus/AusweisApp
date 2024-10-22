import common.Release
import common.Constants
import static common.Constants.strip


// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPK)
{

def j = new Release
	(
		name: 'Android_APK_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'libs/Toolchain_*,build/dist/**,build/src/libAusweisApp*'
	).generate(this)


j.with
{
	parameters
	{
		stringParam("ANDROID_VERSION_CODE", "0", "See https://play.google.com/apps/publish/")
		booleanParam("BUILD_PREVIEW", false, "Use com.governikus.ausweisapp2.dev as package name")
	}

	wrappers
	{
		environmentVariables
		{
			env('QT_ANDROID_KEYSTORE_PATH', '${APK_SIGN_KEYSTORE}')
			env('QT_ANDROID_KEYSTORE_ALIAS', '${APK_SIGN_KEYSTORE_ALIAS}')
			env('QT_ANDROID_KEYSTORE_STORE_PASS', '${APK_SIGN_KEYSTORE_PSW}')
		}
	}

	steps
	{
		buildDescription('', 'ANDROID_VERSION_CODE: ${ANDROID_VERSION_CODE}<br>BUILD_PREVIEW: ${BUILD_PREVIEW}')

		shell(strip("""\
			cmake -P source/ci.cmake --
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DANDROID_VERSION_CODE=\${ANDROID_VERSION_CODE}
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))
	}
}

}


// ----------------------------------------------------------------- AAR
for(ARCH in Constants.AndroidArchAAR)
{

def j = new Release
	(
		name: 'Android_AAR_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'libs/Toolchain_*,build/dist/**,build/**/debug.symbols/*'
	).generate(this)

j.with
{
	parameters
	{
		booleanParam("BUILD_PREVIEW", false, "Use com.governikus.ausweisapp2.dev as package name")
	}

	steps
	{
		buildDescription('', 'BUILD_PREVIEW: ${BUILD_PREVIEW}')

		shell(strip("""\
			cmake -P source/ci.cmake --
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))
	}
}

}


def build = new Release
	(
		name: 'Android_AAR',
		label: 'Android',
		artifacts: 'build/dist/**'
	)

def j = build.generate(this)

j.with
{
	parameters
	{
		booleanParam("PUBLISH", false, "Publish to maven central")

		for(ARCH in Constants.AndroidArchAAR)
		{
			buildSelectorParam(build.getSourceJobNameParam('Android_AAR_' + ARCH))
			{
				defaultBuildSelector
				{
					latestSuccessful(true)
				}
				description('Build of ' + ARCH)
			}
		}
	}

	steps
	{
		for(ARCH in Constants.AndroidArchAAR)
		{
			copyArtifacts(build.getSourceJobName('Android_AAR_' + ARCH))
			{
				flatten()
				buildSelector
				{
					buildParameter(build.getSourceJobNameParam('Android_AAR_' + ARCH))
				}
			}
		}

		shell('cmake -P source/ci.cmake')
	}
}
