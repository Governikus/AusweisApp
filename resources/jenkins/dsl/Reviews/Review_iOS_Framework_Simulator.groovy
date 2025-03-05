import common.Review

def j = new Review
	(
		name: 'iOS_Framework_Simulator',
		libraries: 'iOS_Simulator',
		label: 'iOS',
		artifacts: 'build/*.zip,build/*.bcsymbolmap'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
