import common.Library

def j = new Library
	(
		name: 'Win64_GNU_dev',
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
		batchFile('cd source/libs & cmake --preset ci-gnu-debug')

		shell('''\
			#!c:\\msys64\\usr\\bin\\bash --login
			cd /jenkins/$JOB_NAME/
			cmake --build build --target openssl
			'''.stripIndent().trim())

		batchFile('cmake --build build --target compress')
	}
}
