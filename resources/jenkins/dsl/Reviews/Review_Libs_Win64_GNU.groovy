import common.LibraryReview

def j = new LibraryReview
	(
		name: 'Win64_GNU',
		label: 'Windows',
		weight: 3
	).generate(this)


j.with
{
	customWorkspace('workspace/' + MERCURIAL_REVISION_BRANCH + '_RLW64G')

	wrappers
	{
		environmentVariables
		{
			env('MSYS2_PATH_TYPE', 'inherit')
			env('PATH', '$WORKSPACE/build/dist/bin;$PATH')
		}
	}

	steps
	{
		batchFile('cmake -DPATCH_ONLY=ON -P source/ci.cmake')

		batchFile('cd source/libs & cmake --preset ci-gnu-release')

		shell('''\
			#!c:\\msys64\\usr\\bin\\bash --login
			cd /jenkins/${MERCURIAL_REVISION_BRANCH}_RLW64G/
			cmake --build build --target openssl
			'''.stripIndent().trim())

		batchFile('cmake --build build --target compress')
	}
}
