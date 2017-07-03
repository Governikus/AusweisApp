import common.Review
import common.Constants
import static common.Constants.strip

for(ARCH in Constants.AndroidArch)
{

def j = new Review
	(
		name: 'Android_APK_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/bin/AusweisApp2-*.apk'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell(strip("""\
			cd build;
			cmake ../source
			-DCMAKE_BUILD_TYPE=debug
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake
			-DNDK_CCACHE=/usr/bin/ccache
			-DANDROID_ABI=${ARCH}
			"""))

		shell('cd build; make \${MAKE_FLAGS} install')
		shell('cd build; make apk')
		shell('cd build; make verify.signature')
	}
}

}
