/*!
 * \brief Implementation of ChipAuthenticationInfo
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
DECLARE_ASN1_OBJECT(chipauthenticationinfo_st)


/*
 * Wrapper for structure chipauthenticationinfo_st.
 */
class ChipAuthenticationInfo
	: public SecurityInfo
{
	friend class QSharedPointer<const ChipAuthenticationInfo>;

	private:
		const QSharedPointer<const chipauthenticationinfo_st> mDelegate;

		explicit ChipAuthenticationInfo(const QSharedPointer<const chipauthenticationinfo_st>& pDelegate);
		[[nodiscard]] ASN1_OBJECT* getProtocolObjectIdentifier() const override;
		static bool acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier);

	public:
		static QSharedPointer<const ChipAuthenticationInfo> decode(const QByteArray& pBytes)
		{
			if (const auto& delegate = decodeObject<chipauthenticationinfo_st>(pBytes, false))
			{
				if (ChipAuthenticationInfo::acceptsProtocol(delegate->mProtocol))
				{
					return QSharedPointer<const ChipAuthenticationInfo>::create(delegate);
				}
			}
			return QSharedPointer<const ChipAuthenticationInfo>();
		}


		[[nodiscard]] QByteArray encode() const
		{
			return encodeObject(const_cast<chipauthenticationinfo_st*>(mDelegate.data()));
		}


		[[nodiscard]] int getVersion() const;
		[[nodiscard]] bool hasKeyId() const;
		[[nodiscard]] int getKeyId() const;
};


inline QDebug operator<<(QDebug pDbg, const QSharedPointer<const ChipAuthenticationInfo>& pChipAuthenticationInfo)
{
	QDebugStateSaver saver(pDbg);
	if (pChipAuthenticationInfo->hasKeyId())
	{
		pDbg.nospace().noquote() << pChipAuthenticationInfo->getOid()
								 << ", version: " << pChipAuthenticationInfo->getVersion()
								 << ", keyId: " << pChipAuthenticationInfo->getKeyId();
	}
	else
	{
		pDbg.nospace().noquote() << pChipAuthenticationInfo->getOid()
								 << ", version: " << pChipAuthenticationInfo->getVersion();
	}
	return pDbg;
}


}  // namespace governikus
