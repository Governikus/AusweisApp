import common.LibraryReview
import common.Constants

for(ARCH in Constants.AndroidArch)
{

def j = new LibraryReview
	(
		name: 'Android_' + ARCH,
		label: 'Android'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell("cd build; cmake ../source/libs -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DPACKAGES_DIR=\${PACKAGES_DIR} -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")

		shell('cd build; make compress')
	}
}

}

