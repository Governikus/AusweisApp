import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'MacOS_DMG_PKG',
		libraries: ['MacOS'],
		label: 'MacOS',
		artifacts: 'build/*.dmg,build/*.pkg'
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
			-DJENKINS_APPCAST=${MERCURIAL_REVISION_BRANCH}_Appcast
			"""))

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			cd build; make ${MAKE_FLAGS} package
			'''.stripIndent().trim())

		shell('cd build/_CPack_Packages/Darwin/; codesign -vvvv **/**/*.app')
		shell('cd build/_CPack_Packages/Darwin/DragNDrop; spctl -a -vv **/*.app')

		shell('cd build/; cmake -P ../source/cmake/Notarization.cmake')
	}
}
