import common.Build
import common.Constants
import static common.Constants.strip

for(ARCH in Constants.AndroidArch)
{

def j = new Build
	(
		name: 'Android_APK_' + ARCH,
		libraries: 'Android_' + ARCH,
		label: 'Android',
		artifacts: 'build/dist/**/AusweisApp2-*.apk'
	).generate(this)


j.with
{
	steps
	{
		shell(strip("""\
			cd build;
			cmake ../source
			-DCMAKE_BUILD_TYPE=release
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
}

}
