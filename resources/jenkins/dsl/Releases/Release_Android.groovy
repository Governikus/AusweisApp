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
		artifacts: 'build/dist/**/AusweisApp2-*.apk,build/debug.symbols/**/libAusweisApp2.so'
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
			cd build; cmake ../source
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DAPK_SIGN_KEYSTORE=\${APK_SIGN_KEYSTORE}
			-DAPK_SIGN_KEYSTORE_ALIAS=\${APK_SIGN_KEYSTORE_ALIAS}
			-DAPK_SIGN_KEYSTORE_PSW=\${APK_SIGN_KEYSTORE_PSW}
			-DANDROID_VERSION_CODE=\${ANDROID_VERSION_CODE}
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))

		shell('cd build; make \${MAKE_FLAGS} install')
		shell('cd build; make apk')
		shell('cd build; make verify.signature')
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
		artifacts: 'build/**/dist/**/ausweisapp-*.aar,build/**/dist/**/ausweisapp-*.pom,build/**/dist/**/ausweisapp-*.jar,build/dist/libs/**/debug.symbols/libAusweisApp2.so'
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
	}
}

for(ARCH in Constants.AndroidArchAAR)
{

j.with
{
	steps
	{
		shell(strip("""\
			mkdir -p build/${ARCH};
			cd build/${ARCH};
			cmake ../../source
			-DANDROID_BUILD_AAR=ON
			-DCMAKE_INSTALL_PREFIX=\${WORKSPACE}/build/dist
			-DCMAKE_PREFIX_PATH="\${WORKSPACE}/libs/${ARCH}/build/dist;\${WORKSPACE}/libs/build/dist"
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DBUILD_PREVIEW=\${BUILD_PREVIEW}
			"""))

		shell("cd build/${ARCH}; make \${MAKE_FLAGS} install")
		shell("cd build/${ARCH}; make aar")
	}
}

}
