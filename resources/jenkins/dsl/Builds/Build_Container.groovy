import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Container',
		label: 'Docker',
		artifacts: 'build/AusweisApp*.tar'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
