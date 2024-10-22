import common.Review

def j = new Review
	(
		name: 'Docs',
		label: 'Docs',
		artifacts: 'build/*.pdf,build/docs/**/*.pdf,build/docs/**/*.tar.xz,build/docs/notes/singlehtml/**/appcast.html,build/docs/**/*.inv'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
