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
			cd source; cmake --preset ci-android-apk
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DANDROID_VERSION_CODE=\${ANDROID_VERSION_CODE}
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))

		shell('cmake --build build')
		shell('cmake --build build --target apk')
		shell('cmake --build build --target verify.signature')
		shell('cmake --build build --target dump.apk')
	}
}

}


// ----------------------------------------------------------------- AAR
for(ARCH in Constants.AndroidArchAAR)
{

def j = new Release
	(
		name: 'Android_AAR',
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
			cd source; cmake --preset ci-android-aar
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))

		shell('cmake --build build')
		shell('cmake --build build --target aar')
		shell("cd build/dist; cmake -DCMD=DEPLOY_NEXUS -P \$WORKSPACE/source/cmake/cmd.cmake")
	}
}

}
