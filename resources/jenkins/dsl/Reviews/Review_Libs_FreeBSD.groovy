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
		shell('cmake -DPATCH_ONLY=ON -P source/ci.cmake')

		shell('cd source/libs; cmake --preset ci-debug')

		shell('cmake --build build --target compress')
	}
}
