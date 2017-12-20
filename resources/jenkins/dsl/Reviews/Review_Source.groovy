import common.Review

def j = new Review
	(
		name: 'Source',
		label: 'Common',
		artifacts: 'build/*.tar.gz'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell('cd build; cmake -Werror=dev ../source -DCMAKE_BUILD_TYPE=release -Dtools.only=true')
		shell('cd build; make package_source')
	}
}
