/*!
 * \brief Implementation of PACEInfo
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "SecurityInfo.h"

class test_PaceInfo;

namespace governikus
{

/**
 * PACEInfo ::= SEQUENCE {
 *	      protocol OBJECT IDENTIFIER( id-PACE-DH-GM-3DES-CBC-CBC | id-PACE-DH-GM-AES-CBC-CMAC-128 |
 *	                                           id-PACE-DH-GM-AES-CBC-CMAC-192 | id-PACE-DH-GM-AES-CBC-CMAC-256 |
 *	                                           id-PACE-ECDH-GM-3DES-CBC-CBC | id-PACE-ECDH-GM-AES-CBC-CMAC-128 |
 *	                                           id-PACE-ECDH-GM-AES-CBC-CMAC-192 | id-PACE-ECDH-GM-AES-CBC-CMAC-256 |
 *	                                           id-PACE-DH-IM-3DES-CBC-CBC | id-PACE-DH-IM-AES-CBC-CMAC-128 |
 *	                                           id-PACE-DH-IM-AES-CBC-CMAC-192 | id-PACE-DH-IM-AES-CBC-CMAC-256 |
 *	                                           id-PACE-ECDH-IM-3DES-CBC-CBC | id-PACE-ECDH-IM-AES-CBC-CMAC-128 |
 *	                                           id-PACE-ECDH-IM-AES-CBC-CMAC-192 | id-PACE-ECDH-IM-AES-CBC-CMAC-256),
 *	      version INTEGER, -- SHOULD be 2
 *	      parameterId INTEGER OPTIONAL
 * }
 *
 * defined in TR 3110 Part 3
 */
struct paceinfo_st
{
	ASN1_OBJECT* mProtocol;
	ASN1_INTEGER* mVersion;
	ASN1_INTEGER* mParameterId;
};
DECLARE_ASN1_FUNCTIONS(paceinfo_st)
DECLARE_ASN1_OBJECT(paceinfo_st)


/*
 * Wrapper for structure paceinfo_st.
 */
class PaceInfo
	: public SecurityInfo
{
	friend class QSharedPointer<const PaceInfo>;

	friend class ::test_PaceInfo;

	private:
		const QSharedPointer<const paceinfo_st> mDelegate;

		explicit PaceInfo(const QSharedPointer<const paceinfo_st>& pDelegate);
		[[nodiscard]] ASN1_OBJECT* getProtocolObjectIdentifier() const override;
		[[nodiscard]] static int getMappedNid(int pCurveIndex);
		static bool acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier);

	public:
		static QSharedPointer<const PaceInfo> decode(const QByteArray& pBytes);

		[[nodiscard]] int getVersion() const;
		[[nodiscard]] bool hasParameterId() const;
		[[nodiscard]] int getParameterId() const;
		[[nodiscard]] int getParameterIdAsNid() const;
		[[nodiscard]] bool isStandardizedDomainParameters() const;
};


inline QDebug operator<<(QDebug pDbg, const QSharedPointer<const PaceInfo>& pPaceInfo)
{
	QDebugStateSaver saver(pDbg);
	if (pPaceInfo->hasParameterId())
	{
		pDbg.nospace().noquote() << pPaceInfo->getOid()
								 << ", version: " << pPaceInfo->getVersion()
								 << ", parameterId: " << pPaceInfo->getParameterId();
	}
	else
	{
		pDbg.nospace().noquote() << pPaceInfo->getOid()
								 << ", version: " << pPaceInfo->getVersion();
	}
	return pDbg;
}


}  // namespace governikus
