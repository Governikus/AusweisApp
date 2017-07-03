import common.Build

def j = new Build
	(
		name: 'Source',
		label: 'Common',
		artifacts: 'build/*.tar.gz'
	).generate(this)


j.with
{
	steps
	{
		shell('cd build; cmake ../source -DCMAKE_BUILD_TYPE=release -Dtools.only=true')
		shell('cd build; make package_source')
	}
}
