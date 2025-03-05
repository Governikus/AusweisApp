import common.Build

def j = new Build
	(
		name: 'iOS_Framework',
		libraries: 'iOS',
		label: 'iOS',
		artifacts: 'build/*.zip,build/*.bcsymbolmap',
		trigger: ''
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
