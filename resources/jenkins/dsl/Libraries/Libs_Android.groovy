import common.Library
import common.Constants

for(ARCH in Constants.AndroidArch)
{

def j = new Library
	(
		name: 'Android_' + ARCH,
		label: 'Android'
	).generate(this)


j.with
{
	steps
	{
		shell("cd build; cmake ../source/libs -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake -DPACKAGES_DIR=\${PACKAGES_DIR} -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")

		shell('cd build; make compress')
	}
}

}

