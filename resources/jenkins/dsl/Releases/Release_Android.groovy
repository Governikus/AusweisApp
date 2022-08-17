import common.Release
import common.Constants
import static common.Constants.strip


// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPK)
{

def j = new Release
	(
		name: 'Android_APK_' + ARCH,
		libraries: ['Android_' + ARCH],
		label: 'Android',
		artifacts: 'libs/build/Toolchain_*,build/dist/**/AusweisApp2-*.apk*,build/debug.symbols/*'
	).generate(this)


j.with
{
	parameters
	{
		stringParam("ANDROID_VERSION_CODE", "0", "See https://play.google.com/apps/publish/")
		booleanParam("BUILD_PREVIEW", false, "Use com.governikus.ausweisapp2.dev as package name")
	}

	steps
	{
		buildDescription('', 'ANDROID_VERSION_CODE: ${ANDROID_VERSION_CODE}<br>BUILD_PREVIEW: ${BUILD_PREVIEW}')

		shell(strip("""\
			cd source; cmake --preset ci-android-apk-release
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DANDROID_VERSION_CODE=\${ANDROID_VERSION_CODE}
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))

		shell('cmake --build build')
		shell('cmake --install build')
		shell('cmake --build build --target apk')
		shell('cmake --build build --target verify.signature')
		shell('cmake --build build --target dump.apk')
	}
}

}


// ----------------------------------------------------------------- AAR
def neededLibraries = []
for(ARCH in Constants.AndroidArchAAR)
{
	neededLibraries.add('Android_' + ARCH)
}

def j = new Release
	(
		name: 'Android_AAR',
		libraries: neededLibraries,
		label: 'Android',
		artifacts: 'libs/build/Toolchain_*,build/**/dist/**/ausweisapp-*.aar,build/**/dist/**/ausweisapp-*.pom,build/**/dist/**/ausweisapp-*.jar,build/**/debug.symbols/*'
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
		shell('cmake --install build')
		shell('cmake --build build --target aar')
		shell("cd build/dist; cmake -DCMD=DEPLOY_NEXUS -P \$WORKSPACE/source/cmake/cmd.cmake")
	}
}
