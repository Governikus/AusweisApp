find_program(SMB smbclient REQUIRED)
list(APPEND SMB $ENV{ARCHIVE_URL} -s /dev/null -U $ENV{ARCHIVE_USER}%$ENV{ARCHIVE_PSW})

step(${SMB} "-c mkdir $ENV{REV}" OUTPUT output)
if(output MATCHES "NT_STATUS_OBJECT_NAME_COLLISION")
	message(FATAL_ERROR "Directory already exists: $ENV{REV}")
elseif(output MATCHES "NT_STATUS_ACCESS_DENIED")
	message(FATAL_ERROR "Permission denied")
endif()

set(smbscript "${WORKSPACE}/.smbscript")
file(REMOVE "${smbscript}")
file(GLOB files
	"${WORKSPACE}/*.tar.gz*"
	"${WORKSPACE}/*.zip"
	"${WORKSPACE}/*.apk*"
	"${WORKSPACE}/*.ipa"
	"${WORKSPACE}/*.msi"
	"${WORKSPACE}/*.msi.sha*"
	"${WORKSPACE}/AppcastInfo.json"
	"${WORKSPACE}/*.dmg"
	"${WORKSPACE}/*.dmg.sha*"
	"${WORKSPACE}/*.pkg"
	"${WORKSPACE}/*.pkg.sha*"
	"${WORKSPACE}/*-ReleaseNotes.pdf"
	"${WORKSPACE}/*-ReleaseNotes.tar.xz"
	"${WORKSPACE}/*-NetInstallation_Integration_*.pdf"
	"${WORKSPACE}/*-Failure-Codes-*.pdf"
	"${WORKSPACE}/*-SDK.pdf"
	"${WORKSPACE}/*-SDK.tar.xz"
	"${WORKSPACE}/*-Container*.tar"
	"${WORKSPACE}/*-Lizenz.*"
	"${WORKSPACE}/*_BuildDir.tar.*"
)
foreach(file ${files})
	if(IS_DIRECTORY ${file})
		continue()
	endif()
	cmake_path(GET file FILENAME filename)
	file(RELATIVE_PATH sourcefile "${WORKSPACE}" "${file}")
	file(APPEND "${smbscript}" "put ${sourcefile} $ENV{REV}/${filename}\n")
endforeach()

step(${SMB} INPUT_FILE "${smbscript}")
