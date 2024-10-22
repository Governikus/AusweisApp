import common.Build

def j = new Build
	(
		name: 'MacOS_DMG_PKG',
		libraries: 'MacOS',
		label: 'MacOS',
		artifacts: 'build/*.dmg,build/*.pkg'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
