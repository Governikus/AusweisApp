import common.Library

def j = new Library
	(
		name: 'iOS',
		label: 'iOS'
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain \${KEYCHAIN_CREDENTIALS} \${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source/libs; cmake --preset ci-ios')

		shell('cmake --build build --target compress')
	}
}
