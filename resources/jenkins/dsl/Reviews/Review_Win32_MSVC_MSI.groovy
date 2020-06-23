import common.Review

def j = new Review
	(
		name: 'Win32_MSVC_MSI',
		libraries: ['Win32_MSVC'],
		label: 'MSVC',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile('''\
			cd build
			call vcvarsall.bat x86
			cmake -Werror=dev ../source -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_CXX_COMPILER=clcache -DCMAKE_PREFIX_PATH=%WORKSPACE%/libs/build/dist -GNinja -DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE% -DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW% -DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%
			'''.stripIndent().trim())

		batchFile('''\
			cd build
			call vcvarsall.bat x86
			ninja %MAKE_FLAGS% package
			'''.stripIndent().trim())

		batchFile('''\
			cd build
			call vcvarsall.bat x86
			ninja package.sign
			'''.stripIndent().trim())
	}
}
