import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'Container',
		label: 'Docker',
		artifacts: 'build/AusweisApp*.tar'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell(strip('''\
			docker build
			-t dev-docker.govkg.de/ausweisapp/sdk:${BUILD_TAG}
			--build-arg CCACHE_REMOTE_STORAGE="redis://${CCACHE_REMOTE_STORAGE_HOST}"
			source
			'''))

		shell('docker save -o build/AusweisApp-${BUILD_TAG}.tar dev-docker.govkg.de/ausweisapp/sdk:${BUILD_TAG}')

		shell('docker rmi -f dev-docker.govkg.de/ausweisapp/sdk:${BUILD_TAG}')
	}
}
