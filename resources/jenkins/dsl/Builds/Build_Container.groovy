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
	wrappers
	{
		environmentVariables
		{
			env('TAG', 'dev-${MERCURIAL_REVISION_BRANCH}')
		}
	}

	steps
	{
		shell('docker container prune -f')

		shell(strip('''\
			docker build --pull
			-t dev-docker.govkg.de/ausweisapp/sdk:${TAG//-default/""}
			--build-arg CCACHE_REMOTE_STORAGE="redis://${CCACHE_REMOTE_STORAGE_HOST}"
			source
			'''))

		shell('docker save -o build/AusweisApp-${MERCURIAL_REVISION_SHORT}.tar dev-docker.govkg.de/ausweisapp/sdk:${TAG//-default/""}')
		shell('docker push dev-docker.govkg.de/ausweisapp/sdk:${TAG//-default/""}')

		shell('''\
			IMAGES=`docker images --filter "dangling=true" -q | tail -n +50`
			if [ -z "$IMAGES" ]
			then
				echo "No old dangling images"
			else
				echo "Remove dangling images"
				docker rmi -f $IMAGES
			fi
		'''.stripIndent().trim())
	}
}
