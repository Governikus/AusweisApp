import common.Review

def j = new Review
	(
		name: 'Formatting',
		libraries: 'Linux',
		label: 'Common'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('''\
			cd source
			hg revert -a -C
			hg --config extensions.hgext.purge= purge --all
			hg --config extensions.hgext.strip= strip -r 'secret() or draft()' --no-backup --force 2>/dev/null || echo "No changeset stripped"
			'''.stripIndent().trim())

		shell('cd source; cmake --preset ci-tools-with-libs')

		shell('cd source; resources/jenkins/check_formatting.sh')

		shell('cd source; cmake -DCMD=CHECK_FAILURE_CODES -P cmake/cmd.cmake')
	}

	publishers
	{
		textFinder('FORMATTING FAILED', '', true, false, true)
	}
}
