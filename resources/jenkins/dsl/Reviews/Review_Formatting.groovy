import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'Formatting',
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

		shell('cd build; cmake -Werror=dev ../source -Dtools.only=true')

		shell('''\
			cd build
			make format
			cd ../source

			STATUS=$(hg status | wc -c)
			if [ "$STATUS" != "0" ]; then
			  echo 'Current repository state is not formatted!'
			  hg addremove
			  hg diff
			  hg commit -m "fix formatting" -s
			fi

			(hg --config phases.new-commit=secret import -m 'jenkins patch formatting' -d 'today' -u 'jenkins' ../patch.diff)
			if [ "$?" != "0" ]; then
			  echo 'FORMATTING FAILED: Patch cannot be applied'
			  exit 0
			fi

			cd ../build
			make rebuild_cache
			make format
			cd ../source

			STATUS=$(hg status | wc -c)
			if [ "$STATUS" != "0" ]; then
			  echo 'FORMATTING FAILED: Patch is not formatted'
			  hg diff
			  hg revert -a -C
			  exit 0
			fi
			'''.stripIndent().trim())
	}

	publishers
	{
		textFinder('FORMATTING FAILED', '', true, false, true)
	}
}
