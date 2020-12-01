import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'iOS_Framework',
		libraries: ['iOS'],
		label: 'iOS',
		artifacts: 'build/*.zip,build/*.bcsymbolmap'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('security unlock-keychain \${KEYCHAIN_CREDENTIALS} \${HOME}/Library/Keychains/login.keychain-db')

		shell(strip('''\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_BUILD_TYPE=MinSizeRel
			-DCMAKE_PREFIX_PATH=\${WORKSPACE}/libs/build/dist
			-DCMAKE_TOOLCHAIN_FILE=../source/cmake/iOS.toolchain.cmake
			-DINTEGRATED_SDK=ON
			-GXcode
			'''))

		shell('cd build; xcodebuild -configuration MinSizeRel')
		shell('cd build; xcodebuild -configuration MinSizeRel -target zip')
	}
}
