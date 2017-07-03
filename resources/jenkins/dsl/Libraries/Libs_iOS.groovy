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
		shell('security unlock-keychain \${KEYCHAIN_CREDENTIALS} \${HOME}/Library/Keychains/login.keychain')

		shell("cd build; cmake ../source/libs -DCMAKE_TOOLCHAIN_FILE=../source/cmake/iOS.toolchain.cmake -DPACKAGES_DIR=\${PACKAGES_DIR}")

		shell('cd build; make compress')
	}
}
