import common.LibraryReview

def j = new LibraryReview
	(
		name: 'iOS',
		label: 'iOS'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell('security unlock-keychain \${KEYCHAIN_CREDENTIALS} \${HOME}/Library/Keychains/login.keychain')

		shell("cd build; cmake ../source/libs -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=../source/cmake/iOS.toolchain.cmake -DPACKAGES_DIR=\${PACKAGES_DIR}")

		shell('cd build; make compress')
	}
}
