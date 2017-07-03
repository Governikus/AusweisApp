import common.Appcast

def j = new Appcast
	(
		namePrefix: 'Release_',
		tagName: 'release'
	).generate(this)
