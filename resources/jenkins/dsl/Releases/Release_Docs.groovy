import common.Release

def j = new Release
	(
		name: 'Docs',
		label: 'Docs',
		artifacts: 'build/docs/**/*.pdf,build/docs/**/*.tar.xz,build/docs/notes/singlehtml/**/appcast.html'
	).generate(this)


j.with
{
	parameters
	{
		booleanParam("ENABLE_DVCS", true, "Include mercurial tag instead of CMake version number into docs.")
	}

	steps
	{
		shell('cd build; cmake ../source -DCMAKE_BUILD_TYPE=release -Dtools.only=true -DENABLE_DVCS=\${ENABLE_DVCS}')

		shell('cd build; make notes')
		shell('cd build; make notes.latex.pdf')
		shell('cd build/docs/notes/html; cmake -E tar cfJ ../AusweisApp2_ReleaseNotes.tar.xz .')

		shell('cd build; make sdk')
		shell('cd build; make sdk.latex.pdf')
		shell('cd build/docs/sdk/html; cmake -E tar cfJ ../AusweisApp2_SDK.tar.xz .')

		shell('cd build; make inst.latex.pdf')

		shell('cd build; make inte.latex.pdf')
	}
}
