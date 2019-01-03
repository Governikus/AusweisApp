import common.Build
import common.Constants
import static common.Constants.strip


// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPK)
{

def j = new Build
	(
		name: 'Android_APK_' + ARCH,
		libraries: ['Android_' + ARCH],
		label: 'Android',
		artifacts: 'build/dist/**/AusweisApp2-*.apk,build/debug.symbols/**/libAusweisApp2.so'
	).generate(this)


j.with
{
	steps
	{
		shell(strip("""\
			cd build;
			cmake ../source
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			-DAPK_SIGN_KEYSTORE=\${APK_SIGN_KEYSTORE}
			-DAPK_SIGN_KEYSTORE_ALIAS=\${APK_SIGN_KEYSTORE_ALIAS}
			-DAPK_SIGN_KEYSTORE_PSW=\${APK_SIGN_KEYSTORE_PSW}
			"""))

		shell('cd build; make \${MAKE_FLAGS} install')
		shell('cd build; make apk')
		shell('cd build; make verify.signature')
	}

	publishers {
		androidLint('build/dist/build/outputs/lint-results-*.xml')
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
		artifacts: 'build/**/dist/**/ausweisapp-*.aar,build/**/dist/**/ausweisapp-*.pom,build/**/dist/**/ausweisapp-*.jar,build/dist/libs/**/debug.symbols/libAusweisApp2.so'
	).generate(this)


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
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DCMAKE_INSTALL_PREFIX=\${WORKSPACE}/build/dist
			-DCMAKE_PREFIX_PATH="\${WORKSPACE}/libs/${ARCH}/build/dist;\${WORKSPACE}/libs/build/dist"
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			"""))

		shell("cd build/${ARCH}; make \${MAKE_FLAGS} install")
		shell("cd build/${ARCH}; make aar")
	}

	publishers {
		androidLint('build/dist/build/outputs/lint-results-*.xml')
	}
}

}
