######################################## Qt
if(NOT DEFINED QT)
	set(QT 6.10.2)
	set(QT_PATCHES ON)
endif()

if(NOT QT_HASH)
	set(QT_HASH_6.10.2 c3df0f0e421130cc52ed81cb712358804471ce9bd2a41d97828f9f5b1bf7fed2)

	set(QT_HASH ${QT_HASH_${QT}})
endif()



######################################## OpenSSL
if(NOT DEFINED OPENSSL)
	set(OPENSSL 3.6.3)
	set(OPENSSL_PATCHES ON)
endif()

if(NOT OPENSSL_HASH)
	set(OPENSSL_HASH_1.1.1w cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8)
	set(OPENSSL_HASH_3.0.20 c80a01dfc70ece4dc21168932c37739042d404d46ccc81a5986dd75314ecda6f)
	set(OPENSSL_HASH_3.5.6 deae7c80cba99c4b4f940ecadb3c3338b13cb77418409238e57d7f31f2a3b736)
	set(OPENSSL_HASH_3.6.3 243a86649cf6f23eeb6a2ff2456e09e5d77dd9018a54d3d96b0c6bdd6ba6c7f1)

	set(OPENSSL_HASH ${OPENSSL_HASH_${OPENSSL}})
endif()



######################################## llhttp
if(NOT DEFINED LLHTTP)
	set(LLHTTP 9.3.1)
	set(LLHTTP_PATCHES ON)
endif()

if(NOT LLHTTP_HASH)
	set(LLHTTP_HASH_9.3.1 c14a93f287d3dbd6580d08af968294f8bcc61e1e1e3c34301549d00f3cf09365)

	set(LLHTTP_HASH ${LLHTTP_HASH_${LLHTTP}})
endif()
