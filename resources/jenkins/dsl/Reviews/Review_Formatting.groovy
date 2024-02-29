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

		shell('''\
			cd source
			cmake --build ../build --target format

			STATUS=$(hg status | wc -c)
			if [ "$STATUS" != "0" ]; then
			  echo 'Current repository state is not formatted!'
			  hg addremove
			  hg diff
			  hg commit -m "fix formatting" -s
			fi

			(hg --config patch.eol=auto --config phases.new-commit=secret import -m 'jenkins patch formatting' -d 'today' -u 'jenkins' ../patch.diff)
			if [ "$?" != "0" ]; then
			  echo 'FORMATTING FAILED: Patch cannot be applied'
			  exit 0
			fi

			cmake --build ../build --target rebuild_cache
			cmake --build ../build --target format

			STATUS=$(hg status | wc -c)
			if [ "$STATUS" != "0" ]; then
			  echo 'FORMATTING FAILED: Patch is not formatted'
			  hg diff
			  hg revert -a -C
			  exit 0
			fi
			'''.stripIndent().trim())

		shell('cd source; cmake -DCMD=CHECK_FAILURE_CODES -P cmake/cmd.cmake')
	}

	publishers
	{
		textFinder('FORMATTING FAILED', '', true, false, true)
	}
}
