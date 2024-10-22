import common.Release

def j = new Release
	(
		name: 'iOS_Framework',
		libraries: 'iOS',
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
