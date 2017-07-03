import common.Release

def j = new Release
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
