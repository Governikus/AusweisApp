import common.LibraryReview

def j = new LibraryReview
	(
		name: 'MacOS',
		label: 'MacOS'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell("cd build; cmake ../source/libs -DCMAKE_BUILD_TYPE=release -DPACKAGES_DIR=\${PACKAGES_DIR}")

		shell('cd build; make compress')
	}
}
