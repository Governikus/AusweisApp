include(Utils)

file(GLOB zipfile "${WORKSPACE}/*.zip")
cmake_path(GET zipfile FILENAME zipfilename)
message(STATUS "Use zipfile: ${zipfilename}")
check_beta_version(${zipfile} VERSION version)
message(STATUS "Use zipfile version: ${version}")

if(NOT DEFINED ENV{GITHUB_REPOSITORY_FRAMEWORK})
	set(ENV{GITHUB_REPOSITORY_FRAMEWORK} "https://$ENV{GH_TOKEN}@github.com/Governikus/AusweisApp2-SDK-iOS.git")
endif()

set(MAIN master)

step(git clone --depth 1 $ENV{GITHUB_REPOSITORY_FRAMEWORK} repo)
step(${CMAKE_COMMAND} -E make_directory target)
step(${CMAKE_COMMAND} -E tar xf "${zipfile}" CHDIR target)
step(${CMAKE_COMMAND} -E copy_directory repo/.git target/.git)

prepare_gpg(GPG_CMD SH)

step(git -C target config gpg.program ${GPG_CMD})
step(git -C target config user.name "Governikus")
step(git -C target config user.email "ausweisapp2@governikus.de")
step(git -C target config user.signingkey "2D7479A531451088")

step(git -C target add .)
step(git -C target commit -S -m "Add revision: v${version}")
step(git -C target tag -s ${version} -m "Add tag ${version}")

step(git -C target push origin ${MAIN}:${MAIN})
step(git -C target push origin --tags)

step(gh release create ${version} --draft --title "${version}" CHDIR target)
