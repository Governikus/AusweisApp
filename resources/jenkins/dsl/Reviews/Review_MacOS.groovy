import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'MacOS',
		libraries: 'MacOS',
		label: 'MacOS',
		artifacts: 'tmp/AusweisApp2.*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell(strip('''\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DBUILD_SHARED_LIBS=on
			-DSANITIZER=on
			'''))

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			cd build; make \${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			export QT_PLUGIN_PATH=${WORKSPACE}/libs/build/dist/plugins
			export QML2_IMPORT_PATH=${WORKSPACE}/libs/build/dist/qml
			cd build; ctest ${MAKE_FLAGS}
			'''.stripIndent().trim())
	}
}
