import common.Seeder

def branches = []

// You can override slurping of branches with an
// environment variable.
// Example:
// OVERRIDE_BRANCHES = ['default', 'stable']

if(binding.variables.containsKey('OVERRIDE_BRANCHES'))
{
	branches = Eval.me(OVERRIDE_BRANCHES)
	println 'Extract branches from OVERRIDE_BRANCHES: ' + branches
}
else
{
	println 'Try to slurp branches from repository'

	def api = new URL("${MERCURIAL_REPOSITORY_URL}/json-branches/")
	def content = new groovy.json.JsonSlurper().parse(api.newReader())

	content.each
	{
		empty, entry -> entry.each
		{
			if(it.status != 'closed')
			{
				branches << it.branch
			}
		}
	}
}


if(branches.isEmpty())
{
	throw new Exception('Cannot find any branch')
}

for(branch in branches)
{
	def j = new Seeder
		(
			repository: "${MERCURIAL_REPOSITORY_URL}",
			branch: branch,
		).generate(this)

	queue(j.name)
}
