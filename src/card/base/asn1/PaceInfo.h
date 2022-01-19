/*!
 * \brief Implementation of PACEInfo
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "SecurityInfo.h"


namespace governikus
{

/*!
 * Method used for key agreement:
 * * DH, i.e. Diffie-Hellman
 * * ECDH, i.e. elliptic curve Diffie-Hellman
 */
defineEnumType(KeyAgreementType, DH, ECDH)


/*!
 * Method used for mapping:
 * * GM, i.e. generic mapping
 * * IM, i.e. integrated mapping
 */
defineEnumType(MappingType, GM, IM)


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


/*
 * Wrapper for structure paceinfo_st.
 */
class PaceInfo
	: public SecurityInfo
{
	friend class QSharedPointer<PaceInfo>;

	const QSharedPointer<const paceinfo_st> mDelegate;

	explicit PaceInfo(const QSharedPointer<const paceinfo_st>& pDelegate);
	[[nodiscard]] ASN1_OBJECT* getProtocolObjectIdentifier() const override;
	static bool acceptsProtocol(const ASN1_OBJECT* pObjectIdentifier);

	public:
		static QSharedPointer<PaceInfo> decode(const QByteArray& pBytes)
		{
			if (const auto& delegate = decodeObject<paceinfo_st>(pBytes, false))
			{
				if (PaceInfo::acceptsProtocol(delegate->mProtocol))
				{
					return QSharedPointer<PaceInfo>::create(delegate);
				}
			}
			return QSharedPointer<PaceInfo>();
		}


		[[nodiscard]] QByteArray getParameterId() const;
		[[nodiscard]] int getParameterIdAsInt() const;
		[[nodiscard]] int getVersion() const;
		[[nodiscard]] KeyAgreementType getKeyAgreementType() const;
		[[nodiscard]] MappingType getMappingType() const;
		[[nodiscard]] bool isStandardizedDomainParameters() const;
};


template<>
paceinfo_st* decodeAsn1Object<paceinfo_st>(paceinfo_st** pObject, const unsigned char** pData, long pDataLen);


template<>
void freeAsn1Object<paceinfo_st>(paceinfo_st* pObject);


}  // namespace governikus
