/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>

namespace governikus
{

namespace KnownOIDs
{
enum class Base
{
	BSI_DE,
	SIGNED_DATA,
	ID_SECURITY_OBJECT,
	ID_EXTENSIONS
};

enum class CertificateExtensions
{
	ID_DESCRIPTION,
	ID_SECTOR
};

enum class TermsOfUsageType
{
	ID_PLAIN_FORMAT,
	ID_HTML_FORMAT,
	ID_PDF_FORMAT
};

enum class CHATType
{
	ID_IS,
	ID_AT,
	ID_ST
};

enum class AuxilaryData
{
	ID_DATE_OF_BIRTH,
	ID_DATE_OF_EXPIRY,
	ID_COMMUNITY_ID
};

enum class SecurityProtocol
{
	ID_PK,
	ID_TA,
	ID_CA,
	ID_PACE
};

enum class id_ca
{
	DH,
	DH_3DES_CBC_CBC,
	DH_AES_CBC_CMAC_128,
	DH_AES_CBC_CMAC_192,
	DH_AES_CBC_CMAC_256,
	ECDH,
	ECDH_3DES_CBC_CBC,
	ECDH_AES_CBC_CMAC_128,
	ECDH_AES_CBC_CMAC_192,
	ECDH_AES_CBC_CMAC_256
};

enum class id_ta
{
	ECDSA_SHA_1,
	ECDSA_SHA_224,
	ECDSA_SHA_256,
	ECDSA_SHA_384,
	ECDSA_SHA_512
};

enum class id_pk
{
	DH,
	ECDH
};

namespace id_PACE
{
enum class DH
{
	GM,
	GM_3DES_CBC_CBC,
	GM_AES_CBC_CMAC_128,
	GM_AES_CBC_CMAC_192,
	GM_AES_CBC_CMAC_256,
	IM,
	IM_3DES_CBC_CBC,
	IM_AES_CBC_CMAC_128,
	IM_AES_CBC_CMAC_192,
	IM_AES_CBC_CMAC_256,

};

enum class ECDH
{
	GM,
	GM_3DES_CBC_CBC,
	GM_AES_CBC_CMAC_128,
	GM_AES_CBC_CMAC_192,
	GM_AES_CBC_CMAC_256,
	IM,
	IM_3DES_CBC_CBC,
	IM_AES_CBC_CMAC_128,
	IM_AES_CBC_CMAC_192,
	IM_AES_CBC_CMAC_256
};

}  // namespace KnownOIDs::id_PACE

}  // namespace KnownOIDs

#define DEFINE_TO_BYTE_ARRAY(type)\
	QByteArray toByteArray(type pValue);\
\
	inline QByteArray operator+(type pEnum, const QByteArray& pValue)\
	{\
		return toByteArray(pEnum) + pValue;\
	}\
\
	inline bool operator==(const QByteArray& pValue, type pEnum)\
	{\
		return toByteArray(pEnum) == pValue;\
	}\
\
	inline bool operator!=(const QByteArray& pValue, type pEnum)\
	{\
		return !(pValue == pEnum);\
	}


DEFINE_TO_BYTE_ARRAY(KnownOIDs::Base)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::CertificateExtensions)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::TermsOfUsageType)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::CHATType)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::AuxilaryData)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::SecurityProtocol)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::id_ca)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::id_ta)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::id_pk)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::id_PACE::DH)
DEFINE_TO_BYTE_ARRAY(KnownOIDs::id_PACE::ECDH)

#undef DEFINE_TO_BYTE_ARRAY

}  // namespace governikus
