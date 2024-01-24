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
		shell('cd source; cmake --preset ci-tools')

		shell('cmake --build build --target notes')
		shell('cmake --build build --target notes.latex.pdf')
		shell('cd build/docs/notes; cmake -E tar cfJ ../AusweisApp_ReleaseNotes.tar.xz .')

		shell('cmake --build build --target sdk')
		shell('cmake --build build --target sdk.latex.pdf')
		shell('cd build/docs/sdk/html; cmake -E tar cfJ ../../AusweisApp_SDK.tar.xz .')

		shell('cmake --build build --target failurecodes')
		shell('cmake --build build --target failurecodes.latex.pdf')

		shell('cmake --build build --target installation_integration.latex.pdf')

		shell('cmake --build build --target license')

		shell('cmake --build build --target doc8')
	}
}
