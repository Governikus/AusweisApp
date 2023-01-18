import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Docker_VNC',
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
			docker build --pull
			-t dev-docker.govkg.de/ausweisapp2/vnc:${TAG//-default/""}
			--build-arg CCACHE_REMOTE_STORAGE="redis://${CCACHE_REMOTE_STORAGE_HOST}|share-hits=false"
			-f source/resources/jenkins/docker/Dockerfile
			source
			'''))

		shell('docker run --rm dev-docker.govkg.de/ausweisapp2/vnc:${TAG//-default/""} AusweisApp2 --help')
		shell('docker save -o build/AusweisApp2-VNC-${MERCURIAL_REVISION_SHORT}.tar dev-docker.govkg.de/ausweisapp2/vnc:${TAG//-default/""}')
		shell('docker push dev-docker.govkg.de/ausweisapp2/vnc:${TAG//-default/""}')

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
