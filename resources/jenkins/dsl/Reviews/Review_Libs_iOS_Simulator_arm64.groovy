import common.LibraryReview

def j = new LibraryReview
	(
		name: 'iOS_Simulator_arm64',
		label: 'iOS'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -DPATCH_ONLY=ON -P source/ci.cmake')

		shell('security unlock-keychain \${KEYCHAIN_CREDENTIALS} \${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source/libs; cmake --preset ci-ios-simulator-arm64')

		shell('cmake --build build --target compress')
	}
}
