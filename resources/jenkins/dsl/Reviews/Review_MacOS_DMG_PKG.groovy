import common.Review

def j = new Review
	(
		name: 'MacOS_DMG_PKG',
		libraries: 'MacOS',
		label: 'MacOS',
		artifacts: 'build/*.dmg*,build/*.pkg*'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake -- -DOSX_TIMESTAMP=OFF')
	}
}
