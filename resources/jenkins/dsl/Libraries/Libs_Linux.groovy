import common.Library

def j = new Library
	(
		name: 'Linux',
		label: 'Linux'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source/libs; cmake --preset ci-release')

		shell('cmake --build build --target compress')
	}
}
