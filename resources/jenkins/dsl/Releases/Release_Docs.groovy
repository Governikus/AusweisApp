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
		shell('cd source; cmake --preset ci-tools -DENABLE_DVCS=\${ENABLE_DVCS}')

		shell('cmake --build build --target notes')
		shell('cmake --build build --target notes.latex.pdf')
		shell('cd build/docs/notes; cmake -E tar cfJ ../AusweisApp2_ReleaseNotes.tar.xz .')

		shell('cmake --build build --target sdk')
		shell('cmake --build build --target sdk.latex.pdf')
		shell('cd build/docs/sdk/html; cmake -E tar cfJ ../../AusweisApp2_SDK.tar.xz .')

		shell('cmake --build build --target inst.latex.pdf')

		shell('cmake --build build --target inte.latex.pdf')

		shell('cmake --build build --target license')
	}
}
