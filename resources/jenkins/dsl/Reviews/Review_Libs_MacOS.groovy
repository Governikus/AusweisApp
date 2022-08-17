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
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('cd source/libs; cmake --preset ci-release')

		shell('cmake --build build --target compress')
	}
}
