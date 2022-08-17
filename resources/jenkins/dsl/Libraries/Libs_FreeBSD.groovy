import common.Library

def j = new Library
	(
		name: 'FreeBSD',
		label: 'FreeBSD'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source/libs; cmake --preset ci-debug')

		shell('cmake --build build --target compress')
	}
}
