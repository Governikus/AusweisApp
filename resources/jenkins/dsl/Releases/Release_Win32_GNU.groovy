import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'Win32_GNU_MSI',
		libraries: 'Win32_GNU',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	parameters
	{
		nodeParam('BuildHost')
		{
			description('Jenkins slave')
			defaultNodes(['Windows7_stable'])
			allowedNodes(['Windows7_default', 'Windows7_stable'])
		}
	}

	steps
	{
		batchFile(strip("""\
			cd build &
			cmake ../source -G\"MinGW Makefiles\"
			-DCMAKE_PREFIX_PATH=%WORKSPACE%\\libs\\build\\dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_BUILD_TYPE=release
			-DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE%
			-DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW%
			-DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%
			-DSELFPACKER=true
			"""))

		batchFile('cd build & mingw32-make %MAKE_FLAGS% package')

		batchFile('cd build & mingw32-make package.sign')
	}
}
