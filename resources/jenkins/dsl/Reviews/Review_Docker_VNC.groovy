import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'Docker_VNC',
		label: 'Docker',
		artifacts: 'build/AusweisApp2*.tar'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell(strip('''\
			docker build
			-t dev-docker.govkg.de/ausweisapp2/vnc:${BUILD_TAG}
			--build-arg CCACHE_REMOTE_STORAGE="redis://${CCACHE_REMOTE_STORAGE_HOST}|share-hits=false"
			-f source/resources/jenkins/docker/Dockerfile
			source
			'''))

		shell('docker run --rm dev-docker.govkg.de/ausweisapp2/vnc:${BUILD_TAG} AusweisApp2 --help')
		shell('docker save -o build/AusweisApp2-VNC-${BUILD_TAG}.tar dev-docker.govkg.de/ausweisapp2/vnc:${BUILD_TAG}')

		shell('docker rmi -f dev-docker.govkg.de/ausweisapp2/vnc:${BUILD_TAG}')
	}
}
