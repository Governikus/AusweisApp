import common.Release

def j = new Release
	(
		name: 'Docs',
		label: 'Docs',
		artifacts: 'build/*.pdf,build/docs/**/*.pdf,build/docs/**/*.tar.xz,build/docs/notes/singlehtml/**/appcast.html,build/docs/**/*.inv'
	).generate(this)


j.with
{
	parameters
	{
		booleanParam("ENABLE_DVCS", true, "Include mercurial tag instead of CMake version number into docs.")
	}

	steps
	{
		shell('cmake -P source/ci.cmake -- -DENABLE_DVCS=\${ENABLE_DVCS}')
	}
}
