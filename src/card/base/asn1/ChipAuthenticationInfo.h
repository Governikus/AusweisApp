/*!
 * \brief Implementation of ChipAuthenticationInfo
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "SecurityInfo.h"


namespace governikus
{

/**
 * ChipAuthenticationInfo ::= SEQUENCE {
 *    protocol OBJECT IDENTIFIER( id-CA-DH-3DES-CBC-CBC | id-CA-DH-AES-CBC-CMAC-128 | id-CA-DH-AES-CBC-CMAC-192 |
 *                                 id-CA-DH-AES-CBC-CMAC-256 | id-CA-ECDH-3DES-CBC-CBC | id-CA-ECDH-AES-CBC-CMAC-128 |
 *                                 id-CA-ECDH-AES-CBC-CMAC-192 | id-CA-ECDH-AES-CBC-CMAC-256),
 *    version INTEGER, -- MUST be 1 for CAv1 or 2 for CAv2
 *    keyId INTEGER OPTIONAL
 * }
 *
 * defined in TR 3110 Part 3
 */
struct chipauthenticationinfo_st
{
	ASN1_OBJECT* mProtocol;
	ASN1_INTEGER* mVersion;
	ASN1_INTEGER* mKeyId;
};
DECLARE_ASN1_FUNCTIONS(chipauthenticationinfo_st)


/*
 * Wrapper for structure chipauthenticationinfo_st.
 */
class ChipAuthenticationInfo
	: public SecurityInfo
{
	friend class QSharedPointer<ChipAuthenticationInfo>;

	const QSharedPointer<const chipauthenticationinfo_st> mDelegate;

	explicit ChipAuthenticationInfo(const QSharedPointer<const chipauthenticationinfo_st>& pDelegate);
	ASN1_OBJECT* getProtocolObjectIdentifier() const override;
	static bool acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier);

	public:
		static QSharedPointer<ChipAuthenticationInfo> decode(const QByteArray& pBytes)
		{
			if (const auto& delegate = decodeObject<chipauthenticationinfo_st>(pBytes, false))
			{
				if (ChipAuthenticationInfo::acceptsProtocol(delegate->mProtocol))
				{
					return QSharedPointer<ChipAuthenticationInfo>::create(delegate);
				}
			}
			return QSharedPointer<ChipAuthenticationInfo>();
		}


		QByteArray getVersion() const;
		QByteArray getKeyId() const;
};


template<>
chipauthenticationinfo_st* decodeAsn1Object<chipauthenticationinfo_st>(chipauthenticationinfo_st** pObject, const unsigned char** pData, long pDataLen);


template<>
void freeAsn1Object<chipauthenticationinfo_st>(chipauthenticationinfo_st* pObject);


}  // namespace governikus
