import common.Release

def j = new Release
	(
		name: 'iOS_Framework_Simulator_arm64',
		libraries: 'iOS_Simulator_arm64',
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
