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

		shell("cd source/libs; cmake --preset ci-android -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")

		shell('cmake --build build --target compress')
	}
}

}

