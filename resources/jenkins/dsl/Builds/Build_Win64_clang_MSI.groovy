import common.Build

// ----------------------------------------------------------------- clang-cl
def j = new Build
	(
		name: 'Win64_clang_MSI',
		libraries: 'Win64_MSVC',
		label: 'MSVC',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake -- -DCMAKE_CXX_COMPILER=clang-cl')
	}
}


// ----------------------------------------------------------------- clang++
j = new Build
	(
		name: 'Win64_clang++_MSI',
		libraries: 'Win64_MSVC',
		label: 'MSVC',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake -- -DCMAKE_CXX_COMPILER=clang++')
	}
}
