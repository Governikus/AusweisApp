import common.Release

def j = new Release
	(
		name: 'iOS_Framework_Simulator',
		libraries: 'iOS_Simulator',
		label: 'iOS',
		artifacts: 'libs/Toolchain_*,build/*.zip,build/*.bcsymbolmap,*.tar.zstd'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
