import common.Review

def j = new Review
	(
		name: 'iOS_IPA',
		libraries: 'iOS',
		label: 'iOS',
		artifacts: 'build/*.ipa,build/*.zip,build/*.bcsymbolmap'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
