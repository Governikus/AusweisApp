import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'Container',
		label: 'Docker',
		artifacts: 'build/AusweisApp*.tar'
	).generate(this)


j.with
{
	parameters
	{
		booleanParam("LATEST", false, "Use latest tag")
	}

	steps
	{
		shell(strip('''\
			docker build --pull
			-t dev-docker.govkg.de/ausweisapp/sdk:${changeset}
			--build-arg CCACHE_REMOTE_STORAGE="redis://${CCACHE_REMOTE_STORAGE_HOST}"
			source
			'''))

			shell('docker save -o build/AusweisApp-${changeset}.tar dev-docker.govkg.de/ausweisapp/sdk:${changeset}')
			shell('docker push dev-docker.govkg.de/ausweisapp/sdk:${changeset}')

		conditionalSteps
		{
			condition
			{
				booleanCondition('${LATEST}')
			}

			steps
			{
				shell('docker tag dev-docker.govkg.de/ausweisapp/sdk:${changeset} dev-docker.govkg.de/ausweisapp/sdk:latest')
				shell('docker push dev-docker.govkg.de/ausweisapp/sdk:latest')
			}
		}
	}
}
