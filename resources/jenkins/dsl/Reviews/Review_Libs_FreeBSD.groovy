import common.LibraryReview

def j = new LibraryReview
	(
		name: 'FreeBSD',
		label: 'FreeBSD'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('cd source/libs; cmake --preset ci-debug')

		shell('cmake --build build --target compress')
	}
}
