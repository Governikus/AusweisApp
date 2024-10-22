import common.Library
import common.Constants

for(ARCH in Constants.AndroidArchLibs)
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
		shell("cd source/libs; cmake --preset ci-android -DCMAKE_ANDROID_ARCH_ABI=${ARCH}")

		shell('cmake --build build --target compress')
	}
}

}

