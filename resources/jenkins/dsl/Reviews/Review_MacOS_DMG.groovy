import common.Review
import static common.Constants.strip

def j = new Review
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
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('security unlock-keychain ${KEYCHAIN_CREDENTIALS} ${HOME}/Library/Keychains/login.keychain-db')

		shell(strip("""\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DOSX_TIMESTAMP=OFF
			-DJENKINS_APPCAST=${MERCURIAL_REVISION_BRANCH}_Appcast
			"""))

		shell('''\
			export DYLD_FRAMEWORK_PATH=${WORKSPACE}/libs/build/dist/lib
			export DYLD_LIBRARY_PATH=${WORKSPACE}/libs/build/dist/lib
			cd build; make ${MAKE_FLAGS} package
			'''.stripIndent().trim())

		shell('cd build/_CPack_Packages/Darwin/Bundle/; codesign -vvvv **/*.app; spctl -a -vv **/*.app')
	}
}
