import common.Build

def j = new Build
	(
		name: 'FreeBSD',
		libraries: 'FreeBSD',
		label: 'FreeBSD',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
