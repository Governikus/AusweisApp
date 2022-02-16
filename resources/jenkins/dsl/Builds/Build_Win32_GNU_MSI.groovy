import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Win32_GNU_MSI',
		libraries: ['Win32_GNU'],
		label: 'Windows',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env('PATH', '${COMPILER_${MERCURIAL_REVISION_BRANCH}};$PATH')
		}
	}

	steps
	{
		batchFile(strip("""\
			cd build &
			cmake ../source -G\"MinGW Makefiles\"
			-DCMAKE_PREFIX_PATH=%WORKSPACE%\\libs\\build\\dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DJENKINS_APPCAST=${MERCURIAL_REVISION_BRANCH}_Appcast
			-DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE%
			-DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW%
			-DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%
			"""))

		batchFile('cd build & mingw32-make %MAKE_FLAGS% package')

		batchFile('cd build & cmake -DCMD=CHECK_WIX_WARNING -DFILE=./_CPack_Packages/win32/WIX/wix.log -P ../source/cmake/cmd.cmake')

		batchFile('cd build & mingw32-make package.sign')
	}
}
