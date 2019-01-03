import common.Build

def j = new Build
	(
		name: 'Docs',
		label: 'Docs',
		artifacts: 'build/docs/**/*.pdf,build/docs/**/*.tar.xz,build/docs/notes/singlehtml/**/appcast.html'
	).generate(this)


j.with
{
	steps
	{
		shell('cd build; cmake ../source -DCMAKE_BUILD_TYPE=release -Dtools.only=true')

		shell('cd build; make notes')
		shell('cd build; make notes.latex.pdf')
		shell('cd build/docs/notes/html; cmake -E tar cfJ ../AusweisApp2_ReleaseNotes.tar.xz .')

		shell('cd build; make sdk')
		shell('cd build; make sdk.latex.pdf')
		shell('cd build/docs/sdk/html; cmake -E tar cfJ ../AusweisApp2_SDK.tar.xz .')

		shell('cd build; make inst.latex.pdf')

		shell('cd build; make doc8')
	}
}
