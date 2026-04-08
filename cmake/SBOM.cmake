include(sbombuilder)

sbom_generate(
	CREATOR ORGANIZATION "${VENDOR}"
	PACKAGE_LICENSE "EUPL-1.2"
	PACKAGE_NAME "${PROJECT_NAME}"
	PACKAGE_VERSION "${ARTIFACT_VERSION}"
	PACKAGE_FILENAME "${ARTIFACT_FILENAME}"
	PACKAGE_URL "https://ausweisapp.bund.de"
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/sbom/${ARTIFACT_FILENAME}.spdx"
	NAMESPACE "https://spdx.org/spdxdocs/${PROJECT_NAME}-${ARTIFACT_VERSION}"
)

list(APPEND QT_LIBS qtbase qtwebsockets qtscxml qtconnectivity)
if(NOT INTEGRATED_SDK)
	list(APPEND QT_LIBS qtdeclarative qtsvg)
endif()
foreach(entry ${QT_LIBS})
	sbom_add_package(
		${entry}
		LICENSE "LGPL-3.0-only"
		VERSION "${QT_VERSION}"
		SUPPLIER ORGANIZATION "TheQtCompany"
		URL "https://qt.io"
		EXTREF "SECURITY cpe23Type cpe:2.3:a:qt:${entry}:${QT_VERSION}:*:*:*:*:*:*:*"
		EXTREF "SECURITY cpe23Type cpe:2.3:a:qt:qt:${QT_VERSION}:*:*:*:*:*:*:*"
	)
endforeach()

sbom_add_package(
	openssl
	LICENSE "Apache-2.0"
	VERSION "${OPENSSL_VERSION}"
	SUPPLIER ORGANIZATION "OpenSSL Foundation"
	URL https://www.openssl.org/
	EXTREF "SECURITY cpe23Type cpe:2.3:a:openssl:openssl:${OPENSSL_VERSION}:*:*:*:*:*:*:*"
)

sbom_add_package(
	llhttp
	LICENSE "MIT"
	VERSION "${llhttp_VERSION}"
	SUPPLIER ORGANIZATION "Node.js"
	URL https://github.com/nodejs/llhttp/
	EXTREF "SECURITY cpe23Type cpe:2.3:a:llhttp:llhttp:${llhttp_VERSION}:*:*:*:*:*:*:*"
)

sbom_finalize()
