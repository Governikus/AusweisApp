import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Analyze',
		libraries: 'Linux',
		label: 'Linux'
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env("QT_PLUGIN_PATH", '$WORKSPACE/libs/build/dist/plugins')
		}
	}

	steps
	{
		shell('cd build; cmake ../source -DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist -DCMAKE_CXX_COMPILER=c++-analyzer')

		shell('cd build; make cloc.report')

		shell('cd build; make cppcheck.report')

		shell('''\
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			cd build; scan-build -o $WORKSPACE/build/result --keep-empty make ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('cd build/result; mv * report')
	}

	publishers
	{
		tasks('source/**/*.cpp,source/**/*.h,source/**/*.m,source/**/*.mm,source/**/*.qml,source/**/*.js,source/**/*.cmake,source/**/CMakeLists.txt', '', 'FIXME', 'TODO', 'FINDME', true)
		{
			defaultEncoding('UTF-8')
		}

		warnings(['GNU Make + GNU C Compiler (gcc)'])

		publishHtml
		{
			report('build/result/report')
			{
				reportName('Clang-Analyzer')
				allowMissing(true)
				keepAll(true)
				alwaysLinkToLastBuild(false)
			}
		}

		slocCount
		{
			pattern('build/cloc.xml')
			encoding('UTF-8')
			commentIsCode(false)
			buildsInGraph(0)
			ignoreBuildFailure(false)
		}
	}

	configure
	{
		project -> project / 'publishers' << 'org.jenkinsci.plugins.cppcheck.CppcheckPublisher' {
			cppcheckConfig {
				pattern 'build/cppcheck*.xml'
				ignoreBlankFiles 'false'
				allowNoReport 'false'
				configSeverityEvaluation {
					threshold ''
					newThreshold ''
					failureThreshold ''
					newFailureThreshold ''
					healthy ''
					unHealthy
					severityError 'true'
					severityWarning 'true'
					severityStyle 'true'
					severityPerformance 'true'
					severityInformation 'true'
					severityNoCategory 'false'
					severityPortability 'false'
				}
			}
		}
	}
}
