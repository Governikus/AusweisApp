/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <openssl/asn1t.h>
#include <openssl/err.h>

#include <QByteArray>
#include <QLoggingCategory>
#include <QScopeGuard>
#include <QSharedPointer>


namespace governikus
{

const QLoggingCategory& getLoggingCategory();
QByteArray getOpenSslError();

/*!
 * Default template function for creating an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
T* newAsn1Object()
{
	static_assert(std::is_void_v<T>, "Implement specialization of newAsn1Object");
	return 0;
}


/*!
 * Template function for creating a SharedPointer holding an ASN.1 OpenSSL type.
 */
template<typename T>
QSharedPointer<T> newObject(T* pObject = newAsn1Object<T>())
{
	static auto deleter = [](T* pObjectToDelete)
			{
				freeAsn1Object(pObjectToDelete);
			};
	return QSharedPointer<T>(pObject, deleter);
}


/*!
 * Default template function for encoding an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
int encodeAsn1Object(const T*, uchar**)
{
	static_assert(std::is_void_v<T>, "Implement specialization of encodeObject");
	return 0;
}


/*!
 * Template function for encoding an OpenSSL type as DER encoded QByteArray.
 */
template<typename T>
QByteArray encodeObject(T* pObject)
{
	if (!pObject)
	{
		return QByteArray();
	}

	ERR_clear_error();
	uchar* encoded = nullptr;
	const int length = encodeAsn1Object(pObject, &encoded);
	const auto guard = qScopeGuard([encoded] {
				OPENSSL_free(encoded);
			});
	if (length < 0)
	{
		qCWarning(governikus::getLoggingCategory()) << "Cannot encode ASN.1 object:" << getOpenSslError();
		return QByteArray();
	}

	return QByteArray(reinterpret_cast<char*>(encoded), length);
}


/*!
 * Default template function for decoding an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
T* decodeAsn1Object(T**, const uchar**, long)
{
	static_assert(std::is_void_v<T>, "Implement specialization of decodeObject");
	return 0;
}


/*!
 * Default template function for freeing an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
void freeAsn1Object(T*)
{
	static_assert(std::is_void_v<T>, "Implement specialization of freeObject");
}


/*!
 * Template function for decoding an OpenSSL type from DER encoded QByteArray.
 */
template<typename T>
QSharedPointer<T> decodeObject(const QByteArray& pData, bool pLogging = true)
{
	ERR_clear_error();

	T* object = nullptr;
	const auto* dataPointer = reinterpret_cast<const unsigned char*>(pData.constData());
	if (!decodeAsn1Object(&object, &dataPointer, static_cast<long>(pData.length())) && pLogging)
	{
		qCWarning(governikus::getLoggingCategory()) << "Cannot decode ASN.1 object:" << getOpenSslError();
	}

	static auto deleter = [](T* pTypeObject)
			{
				freeAsn1Object(pTypeObject);
			};
	return QSharedPointer<T>(object, deleter);
}


/*!
 * Return value of ASN.1 callback functions indicating success.
 */
static const int CB_SUCCESS = 1;


/*!
 * Return value of ASN.1 callback functions indicating an error.
 */
static const int CB_ERROR = 0;


#if OPENSSL_VERSION_NUMBER < 0x30000000L
	#define i2d_const_cast(name, object) const_cast<name*>(object)
#else
	#define i2d_const_cast(name, object) object
#endif


#define IMPLEMENT_ASN1_OBJECT(name)\
		template<>\
		name * newAsn1Object<name>()\
		{\
			return name##_new();\
		}\
\
		template<>\
		int encodeAsn1Object<name>(const name * pObject, uchar * *encoded)\
		{\
			return i2d_##name(i2d_const_cast(name, pObject), encoded);\
		}\
\
		template<>\
		name * decodeAsn1Object<name>(name * *pObject, const uchar * *pData, long pDataLen)\
		{\
			return d2i_##name(pObject, pData, pDataLen);\
		}\
\
		template<>\
		void freeAsn1Object<name>(name * pObject)\
		{\
			name##_free(pObject);\
		}

#define DECLARE_ASN1_OBJECT(name)\
		template<> name * newAsn1Object<name>();\
		template<> int encodeAsn1Object<name>(const name * pObject, uchar * *encoded);\
		template<> name * decodeAsn1Object<name>(name * *pObject, const uchar * *pData, long pDataLen);\
		template<> void freeAsn1Object<name>(name * pObject);


} // namespace governikus
