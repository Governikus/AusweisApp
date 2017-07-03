/*!
 * \brief Utility template functions for encoding and decoding of ASN.1 types
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <openssl/asn1t.h>
#include <openssl/err.h>

#include <QByteArray>
#include <QDebug>
#include <QSharedPointer>

namespace governikus
{


/*!
 * Default template function for creating an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
T* newAsn1Object()
{
	static_assert(std::is_void<T>::value, "Implement specialization of newAsn1Object");
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
int encodeAsn1Object(T*, unsigned char**)
{
	static_assert(std::is_void<T>::value, "Implement specialization of encodeObject");
	return 0;
}


/*!
 * Template function for encoding an OpenSSL type as DER encoded QByteArray.
 */
template<typename T>
QByteArray encodeObject(T* pObject)
{
	unsigned char* encoded = nullptr;
	int length = encodeAsn1Object(pObject, &encoded);
	if (length < 0)
	{
		BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
		ERR_print_errors(bio_err);
		BIO_free(bio_err);
		length = 0;
	}
	return QByteArray(reinterpret_cast<char*>(encoded), length);
}


/*!
 * Default template function for decoding an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
T* decodeAsn1Object(T**, const unsigned char**, long)
{
	static_assert(std::is_void<T>::value, "Implement specialization of decodeObject");
	return 0;
}


/*!
 * Default template function for freeing an OpenSSL type. This must be specialized for each ASN.1 type.
 */
template<typename T>
void freeAsn1Object(T*)
{
	static_assert(std::is_void<T>::value, "Implement specialization of freeObject");
}


/*!
 * Template function for decoding an OpenSSL type from DER encoded QByteArray.
 */
template<typename T>
QSharedPointer<T> decodeObject(const QByteArray& pData)
{
	{
		const char* tmp = pData.constData();
		const unsigned char** dataPointer = reinterpret_cast<unsigned const char**>(&tmp);

		T* object = nullptr;
		decodeAsn1Object(&object, dataPointer, pData.length());
		static auto deleter = [](T* pTypeObject)
				{
					freeAsn1Object(pTypeObject);
				};
		return QSharedPointer<T>(object, deleter);
	}
}


/*!
 * Return value of ASN.1 callback functions indicating success.
 */
static const int CB_SUCCESS = 1;


/*!
 * Return value of ASN.1 callback functions indicating an error.
 */
static const int CB_ERROR = 0;


#define IMPLEMENT_ASN1_OBJECT(name)\
	template<>\
	name * newAsn1Object<name>()\
	{\
		return name##_new();\
	}\
\
	template<>\
	int encodeAsn1Object<name>(name * pObject, unsigned char** encoded)\
	{\
		return i2d_##name(pObject, encoded);\
	}\
\
	template<>\
	name * decodeAsn1Object<name>(name** pObject, const unsigned char** pData, long pDataLen)\
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
	template<> int encodeAsn1Object<name>(name * pObject, unsigned char** encoded);\
	template<> name * decodeAsn1Object<name>(name** pObject, const unsigned char** pData, long pDataLen);\
	template<> void freeAsn1Object<name>(name * pObject);


}
