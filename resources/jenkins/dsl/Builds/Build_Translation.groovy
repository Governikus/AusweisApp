import common.Build

def j = new Build
	(
		name: 'Translation',
		excludePattern: '',
		libraries: ['Linux'],
		label: 'Linux',
		artifacts: 'source/resources/translations/*.ts'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake --preset ci-translations')
		shell('cmake --build build --target update.translations')
	}
}
