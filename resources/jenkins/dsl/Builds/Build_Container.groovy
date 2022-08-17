import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Container',
		label: 'Docker',
		artifacts: 'build/AusweisApp2*.tar'
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
		shell(strip('''\
			docker build
			-t dev-docker.govkg.de/ausweisapp2/sdk:${TAG//-default/""}
			--build-arg CCACHE_SECONDARY_STORAGE="redis://${CCACHE_SECONDARY_STORAGE_HOST}|share-hits=false"
			source
			'''))

		shell('docker save -o build/AusweisApp2-${MERCURIAL_REVISION_SHORT}.tar dev-docker.govkg.de/ausweisapp2/sdk:${TAG//-default/""}')
		shell('docker push dev-docker.govkg.de/ausweisapp2/sdk:${TAG//-default/""}')

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
