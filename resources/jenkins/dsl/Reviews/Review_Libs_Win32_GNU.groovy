import common.LibraryReview

def j = new LibraryReview
	(
		name: 'Win32_GNU',
		label: 'Windows',
		weight: 3
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env('MSYS2_PATH_TYPE', 'inherit')
			env('PATH', '${COMPILER_${MERCURIAL_REVISION_BRANCH}};$PATH')
		}
	}

	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile("cd build & cmake ../source/libs -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=%PACKAGES_DIR% -G\"MinGW Makefiles\" -DWIN_SIGN_KEYSTORE=%WIN_SIGN_KEYSTORE% -DWIN_SIGN_KEYSTORE_PSW=%WIN_SIGN_KEYSTORE_PSW% -DWIN_SIGN_SUBJECT_NAME=%WIN_SIGN_SUBJECT_NAME%")

		shell('''\
			#!c:\\msys64\\usr\\bin\\bash --login
			cd /jenkins/$JOB_NAME/build
			mingw32-make openssl
			'''.stripIndent().trim())

		batchFile('cd build & mingw32-make compress')
	}
}
