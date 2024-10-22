import common.LibraryReview

def j = new LibraryReview
	(
		name: 'Linux',
		label: 'Linux'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -DPATCH_ONLY=ON -P source/ci.cmake')

		shell('cd source/libs; cmake --preset ci-release')

		shell('cmake --build build --target compress')
	}
}
