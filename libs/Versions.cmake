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
	set(OPENSSL 3.6.1)
	set(OPENSSL_PATCHES ON)
endif()

if(NOT OPENSSL_HASH)
	set(OPENSSL_HASH_1.1.1w cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8)
	set(OPENSSL_HASH_3.0.18 d80c34f5cf902dccf1f1b5df5ebb86d0392e37049e5d73df1b3abae72e4ffe8b)
	set(OPENSSL_HASH_3.5.5 b28c91532a8b65a1f983b4c28b7488174e4a01008e29ce8e69bd789f28bc2a89)
	set(OPENSSL_HASH_3.6.1 b1bfedcd5b289ff22aee87c9d600f515767ebf45f77168cb6d64f231f518a82e)

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
