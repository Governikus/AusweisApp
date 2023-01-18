import common.Release
import static common.Constants.strip

def j = new Release
	(
		name: 'Container',
		label: 'Docker',
		artifacts: 'build/AusweisApp2*.tar'
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
			-t dev-docker.govkg.de/ausweisapp2/sdk:${changeset}
			--build-arg CCACHE_REMOTE_STORAGE="redis://${CCACHE_REMOTE_STORAGE_HOST}|share-hits=false"
			source
			'''))

			shell('docker save -o build/AusweisApp2-${changeset}.tar dev-docker.govkg.de/ausweisapp2/sdk:${changeset}')
			shell('docker push dev-docker.govkg.de/ausweisapp2/sdk:${changeset}')

		conditionalSteps
		{
			condition
			{
				booleanCondition('${LATEST}')
			}

			steps
			{
				shell('docker tag dev-docker.govkg.de/ausweisapp2/sdk:${changeset} dev-docker.govkg.de/ausweisapp2/sdk:latest')
				shell('docker push dev-docker.govkg.de/ausweisapp2/sdk:latest')
			}
		}
	}
}
