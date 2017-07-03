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
	wrappers
	{
		environmentVariables
		{
			env("PATH", "\${WORKSPACE}/build/standalone/bin:${PATH}")
		}
	}

	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell("cd build; cmake ../source/libs -DCMAKE_TOOLCHAIN_FILE=../source/cmake/android.toolchain.cmake -DPACKAGES_DIR=\${PACKAGES_DIR} -DANDROID_ABI=${ARCH}")

		shell('cd build; make compress')
	}
}

}

