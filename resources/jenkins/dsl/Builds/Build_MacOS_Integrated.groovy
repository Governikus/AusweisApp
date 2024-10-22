import common.Build

def j = new Build
	(
		name: 'MacOS_Integrated',
		libraries: 'MacOS',
		label: 'MacOS',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
