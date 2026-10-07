function(SET_HASH _var)
	if(NOT ${_var}_HASH_${${_var}})
		message(FATAL_ERROR "Hash value not defined for ${_var}: ${${_var}}")
	endif()
	set(${_var}_HASH ${${_var}_HASH_${${_var}}} PARENT_SCOPE)
endfunction()

macro(hash _name _version _hash)
	set(${_name}_HASH_${_version} ${_hash})
	list(APPEND ${_name}_VERSIONS ${_version})
endmacro()

######################################## Qt
if(NOT DEFINED QT)
	set(QT 6.11.1)
	set(QT_PATCHES ON)
endif()

if(NOT QT_HASH)
	hash(QT 6.11.1 252acef8c5ae68074d91cadba2ee4a83465051bbb970dd26e8f0daa0f3904e03)

	SET_HASH(QT)
endif()



######################################## OpenSSL
if(NOT DEFINED OPENSSL)
	set(OPENSSL 4.0.3)
	set(OPENSSL_PATCHES ON)
endif()
string(REGEX MATCH "^[0-9]+" OPENSSL_FILE_VERSION ${OPENSSL})

if(NOT OPENSSL_HASH)
	hash(OPENSSL 1.1.1w cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8)
	hash(OPENSSL 3.0.21 617e29af8e421f46649484a4937e48c685e47f46488167c982f88bc4ec1d522f)
	hash(OPENSSL 3.5.7 a8c0d28a529ca480f9f36cf5792e2cd21984552a3c8e4aa11a24aa31aeac98e8)
	hash(OPENSSL 4.0.3 325b5c806167c13b40b1ffeadfe0248197c00eccc4cf123ec1e28d2d2fd216d9)

	SET_HASH(OPENSSL)
endif()



######################################## llhttp
if(NOT DEFINED LLHTTP)
	set(LLHTTP 9.4.3)
	set(LLHTTP_PATCHES ON)
endif()

if(NOT LLHTTP_HASH)
	hash(LLHTTP 9.4.3 1eb813c7437b31a87496a1cd3ed79f00746720f5e7e29c79b42c02cb69f36c39)

	SET_HASH(LLHTTP)
endif()
