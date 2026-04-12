include(Utils)

file(GLOB tarball "${WORKSPACE}/*.tar.gz")
cmake_path(GET tarball FILENAME tarballname)
message(STATUS "Use tarball: ${tarballname}")
check_beta_version(${tarball} VERSION version)
message(STATUS "Use tarball version: ${version}")
string(REGEX MATCH "[0-9]+\\.[0-9]+" MAJOR_MINOR "${version}")
message(STATUS "Use version as branch: ${MAJOR_MINOR}")

if(NOT DEFINED ENV{GITHUB_REPOSITORY_SOURCE})
	set(ENV{GITHUB_REPOSITORY_SOURCE} "https://$ENV{GH_TOKEN}@github.com/Governikus/AusweisApp.git")
endif()
if(NOT DEFINED ENV{OPENCODE_REPOSITORY_SOURCE})
	set(ENV{OPENCODE_REPOSITORY_SOURCE} "https://$ENV{OPENCODE_TOKEN}@gitlab.opencode.de/governikus/ausweisapp.git")
endif()

set(MAIN master)

step(git clone --no-checkout $ENV{GITHUB_REPOSITORY_SOURCE} repo)
step(git -C repo show-ref --verify --quiet refs/remotes/origin/${MAJOR_MINOR} RESULT NO_BRANCH_FOUND)

if(NO_BRANCH_FOUND)
	set(source_branch origin/${MAIN})
else()
	set(source_branch origin/${MAJOR_MINOR})
endif()

step(git -C repo switch -c ${MAJOR_MINOR} ${source_branch})
step(${CMAKE_COMMAND} -E make_directory target)
step(tar xf "${tarball}" --strip-components=1 -C target)
step(${CMAKE_COMMAND} -E copy_directory repo/.git target/.git)

prepare_gpg(GPG_CMD SH)

step(git -C target config gpg.program ${GPG_CMD})
step(git -C target config user.name "Governikus")
step(git -C target config user.email "ausweisapp@governikus.de")
step(git -C target config user.signingkey "2D7479A531451088")

step(git -C target add .)
step(git -C target commit -S -m "Add revision: v${version}")
step(git -C target tag -s ${version} -m "Add tag ${version}")

step(git -C target switch ${MAIN})
step(git -C target merge ${MAJOR_MINOR} --ff-only)

step(git -C target switch community)
step(git -C target merge -X theirs ${MAIN} -S -m "Merge v${version} into community")

step(git -C target remote add opencode $ENV{OPENCODE_REPOSITORY_SOURCE})
step(git -C target fetch opencode)

foreach(entry origin opencode)
	step(git -C target push ${entry} ${MAJOR_MINOR}:${MAJOR_MINOR})
	step(git -C target push ${entry} ${MAIN}:${MAIN})
	step(git -C target push ${entry} community:community)
	step(git -C target push ${entry} --tags)
endforeach()

step(gh release create ${version} --draft --title "${version}" --notes "# Anwender\n* TODO\n\n\n# Entwickler\n* TODO" CHDIR target)
file(GLOB files
	"${WORKSPACE}/*.tar.gz*"
	"${WORKSPACE}/*.apk"
	"${WORKSPACE}/*.apk.sha*"
	"${WORKSPACE}/*.msi"
	"${WORKSPACE}/*.msi.sha*"
	"${WORKSPACE}/*.dmg"
	"${WORKSPACE}/*.dmg.sha*"
	"${WORKSPACE}/*-ReleaseNotes.pdf"
	"${WORKSPACE}/*-NetInstallation_Integration_*.pdf"
	"${WORKSPACE}/*-SDK.pdf"
	"${WORKSPACE}/*-Lizenz.txt"
)
step(gh release upload ${version} ${files} CHDIR target)
