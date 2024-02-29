import common.Build

def j = new Build
	(
		name: 'MacOS_Integrated',
		libraries: 'MacOS',
		label: 'MacOS',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell('cd source; cmake --preset ci-macos-integrated')

		shell('''\
			cmake --build build
			'''.stripIndent().trim())

		shell('''\
			export QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
