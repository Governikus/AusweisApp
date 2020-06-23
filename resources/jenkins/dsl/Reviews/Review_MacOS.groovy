import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'MacOS',
		libraries: ['MacOS'],
		label: 'MacOS',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

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
			export ASAN_OPTIONS=detect_leaks=0,new_delete_type_mismatch=0
			cd build; ctest --output-on-failure ${MAKE_FLAGS}
			'''.stripIndent().trim())
	}
}
