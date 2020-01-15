import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'MacOS_DMG',
		libraries: ['MacOS'],
		label: 'MacOS',
		artifacts: 'build/*.dmg'
	).generate(this)


j.with
{
	steps
	{
		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell(strip("""\
			cd build;
			cmake ../source
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_BUILD_TYPE=MinSizeRel
			"""))

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			cd build; make ${MAKE_FLAGS} package
			'''.stripIndent().trim())

		shell('cd build/_CPack_Packages/Darwin/Bundle/; codesign -vvvv **/*.app; spctl -a -vv **/*.app')

		shell('cd build/; cmake -P ../source/cmake/Notarization.cmake')
	}
}
