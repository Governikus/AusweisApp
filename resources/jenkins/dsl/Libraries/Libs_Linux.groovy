import common.Library

def j = new Library
	(
		name: 'Linux',
		label: 'Linux'
	).generate(this)


j.with
{
	steps
	{
		shell("cd build; cmake ../source/libs -DPACKAGES_DIR=\${PACKAGES_DIR}")

		shell('cd build; make compress')
	}
}
