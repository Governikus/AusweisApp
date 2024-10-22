import common.Build

def j = new Build
	(
		name: 'Linux_Integrated',
		label: 'Vanilla',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
