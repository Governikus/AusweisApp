import common.Appcast

def j = new Appcast
	(
		namePrefix: 'Release_',
		releaseJob: true
	).generate(this)


j.with
{
	parameters
	{
		stringParam('changeset', 'release', 'Build given changeset (tag) as release')
	}

	wrappers
	{
		buildName('${changeset}')
	}
}
