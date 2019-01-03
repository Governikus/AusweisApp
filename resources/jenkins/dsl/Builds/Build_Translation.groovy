import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Translation',
		libraries: ['Linux'],
		label: 'Linux',
		artifacts: 'source/resources/translations/*.ts'
	).generate(this)


j.with
{
	steps
	{
		shell(strip('''\
			cd build;
			cmake ../source
			-DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist
			-DUPDATE_TRANSLATIONS=ON
			-DUPDATE_TRANSLATIONS_NO_OBSOLETE=ON
			'''))

		shell('cd build; make update.translations')
	}
}
