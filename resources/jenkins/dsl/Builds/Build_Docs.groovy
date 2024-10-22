import common.Build

def j = new Build
	(
		name: 'Docs',
		label: 'Docs',
		artifacts: 'build/*.pdf,build/docs/**/*.pdf,build/docs/**/*.tar.xz,build/docs/notes/singlehtml/**/appcast.html,build/docs/**/*.inv'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
