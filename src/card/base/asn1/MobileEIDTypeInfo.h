/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "SecurityInfo.h"


namespace governikus
{

/**
 *
 * MobileEIDTypeInfo ::= SEQUENCE {
 *    protocol  OBJECT IDENTIFIER (
 *                  id-mobileEIDType-SECertified |
 *                  id-mobileEIDType-SEEndorsed |
 *                  id-mobileEIDType-HWKeyStore)
 * }
 *
 * defined in TR 3110 Part 3
 */
struct mobileeidtypeinfo_st
{
	ASN1_OBJECT* mProtocol;
	ASN1_TYPE* mRequiredData;
};
DECLARE_ASN1_FUNCTIONS(mobileeidtypeinfo_st)
DECLARE_ASN1_OBJECT(mobileeidtypeinfo_st)


/*
 * Wrapper for structure mobileeidtypeinfo_st.
 */
class MobileEIDTypeInfo
	: public SecurityInfo
{
	friend class QSharedPointer<const MobileEIDTypeInfo>;

	private:
		const QSharedPointer<const mobileeidtypeinfo_st> mDelegate;

		explicit MobileEIDTypeInfo(const QSharedPointer<const mobileeidtypeinfo_st>& pDelegate);
		[[nodiscard]] ASN1_OBJECT* getProtocolObjectIdentifier() const override;
		static bool acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier);

	public:
		static QSharedPointer<const MobileEIDTypeInfo> decode(const QByteArray& pBytes);
		[[nodiscard]] QByteArray encode() const;
};


inline QDebug operator<<(QDebug pDbg, const QSharedPointer<const MobileEIDTypeInfo>& pMobileEIDTypeInfo)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace().noquote() << pMobileEIDTypeInfo->getOid();
	return pDbg;
}


}  // namespace governikus
