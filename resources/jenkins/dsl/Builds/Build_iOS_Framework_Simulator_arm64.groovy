import common.Build

def j = new Build
	(
		name: 'iOS_Framework_Simulator_arm64',
		libraries: 'iOS_Simulator_arm64',
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
