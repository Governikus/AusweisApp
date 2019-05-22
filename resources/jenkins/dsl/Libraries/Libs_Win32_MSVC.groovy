import common.Library

def j = new Library
	(
		name: 'Win32_MSVC',
		label: 'MSVC'
	).generate(this)


j.with
{
	steps
	{
		batchFile('''\
			cd build
			call vcvarsall.bat x86
			cmake ../source/libs -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=%PACKAGES_DIR% -G"NMake Makefiles" -DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE% -DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW% -DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%
			'''.stripIndent().trim())

		batchFile('''\
			cd build
			call vcvarsall.bat x86
			nmake compress
			'''.stripIndent().trim())
	}
}
