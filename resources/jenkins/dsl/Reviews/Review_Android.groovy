import common.Review
import common.Constants
import static common.Constants.strip


// ----------------------------------------------------------------- APK
for(ARCH in Constants.AndroidArchAPKReview)
{

def j = new Review
	(
		name: 'Android_APK_' + ARCH,
		libraries: ['Android_' + ARCH],
		label: 'Android',
		artifacts: 'build/dist/**/AusweisApp2-*.apk'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell(strip("""\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_BUILD_TYPE=debug
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_ANDROID_ARCH_ABI=${ARCH}
			"""))

		shell('cd build; make \${MAKE_FLAGS} install')
		shell('cd build; make apk')
		shell('cd build; make verify.signature')
	}

	publishers {
		androidLint('build/dist/build/reports/lint-results.xml')
		{
			thresholds(
				unstableTotal: [all: 1]
			)
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

def j = new Review
	(
		name: 'Android_AAR',
		libraries: neededLibraries,
		label: 'Android',
		artifacts: 'build/**/dist/**/ausweisapp-*.aar'
	).generate(this)

j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')
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
			cmake -Werror=dev ../../source
			-DINTEGRATED_SDK=ON
			-DCMAKE_BUILD_TYPE=debug
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
		androidLint('build/dist/build/reports/lint-results.xml')
		{
			thresholds(
				unstableTotal: [all: 0]
			)
		}
	}
}

}
