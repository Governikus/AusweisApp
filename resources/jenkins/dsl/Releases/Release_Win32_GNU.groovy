import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'Win32_GNU_MSI',
		libraries: ['Win32_GNU'],
		label: 'Windows',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile(strip("""\
			cd build &
			cmake ../source -G\"MinGW Makefiles\"
			-DCMAKE_PREFIX_PATH=%WORKSPACE%\\libs\\build\\dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE%
			-DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW%
			-DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%
			"""))

		batchFile('cd build & mingw32-make %MAKE_FLAGS% package')

		batchFile('cd build & mingw32-make package.sign')
	}
}
