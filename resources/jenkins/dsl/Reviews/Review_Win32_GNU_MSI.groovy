import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'Win32_GNU_MSI',
		libraries: 'Win32_GNU',
		label: "Windows7_${MERCURIAL_REVISION_BRANCH}",
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & python resources/jenkins/import.py')

		batchFile(strip("""\
			cd build &
			cmake -Werror=dev ../source -G\"MinGW Makefiles\"
			-DCMAKE_PREFIX_PATH=%WORKSPACE%\\libs\\build\\dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_BUILD_TYPE=release
			-DJENKINS_APPCAST=${MERCURIAL_REVISION_BRANCH}_Appcast
			-DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE%
			-DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW%
			-DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%
			-DSELFPACKER=true
			"""))

		batchFile('cd build & mingw32-make %MAKE_FLAGS% package')

		batchFile('cd build & mingw32-make package.sign')
	}
}
