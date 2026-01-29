######################################## Qt
if(NOT DEFINED QT)
	set(QT 6.9.2)
	set(QT_PATCHES ON)
endif()

if(NOT QT_HASH)
	set(QT_HASH_6.9.2 643f1fe35a739e2bf5e1a092cfe83dbee61ff6683684e957351c599767ca279c)

	set(QT_HASH ${QT_HASH_${QT}})
endif()



######################################## OpenSSL
if(NOT DEFINED OPENSSL)
	set(OPENSSL 3.5.5)
	set(OPENSSL_PATCHES ON)
endif()

if(NOT OPENSSL_HASH)
	set(OPENSSL_HASH_1.1.1w cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8)
	set(OPENSSL_HASH_3.0.18 d80c34f5cf902dccf1f1b5df5ebb86d0392e37049e5d73df1b3abae72e4ffe8b)
	set(OPENSSL_HASH_3.5.5 b28c91532a8b65a1f983b4c28b7488174e4a01008e29ce8e69bd789f28bc2a89)

	set(OPENSSL_HASH ${OPENSSL_HASH_${OPENSSL}})
endif()
