import common.Library

def j = new Library
	(
		name: 'FreeBSD',
		label: 'FreeBSD'
	).generate(this)


j.with
{
	steps
	{
		shell("cd build; cmake ../source/libs -DCMAKE_BUILD_TYPE=debug -DPACKAGES_DIR=\${PACKAGES_DIR}")

		shell('cd build; make compress')
	}
}
