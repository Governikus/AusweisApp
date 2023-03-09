/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * Known OIDs according to TR-03110-3 A.1.1
 */

#pragma once

#include "EnumHelper.h"

#include <QByteArray>
#include <openssl/objects.h>


class test_Oid;


namespace governikus
{

defineEnumType(KnownOid
		, ID_SIGNED_DATA
		, EC_PS_PUBLICKEY
		, STANDARDIZED_DOMAINPARAMETERS
		, BSI_DE_PROTOCOLS_SMARTCARD
		, ID_PK
		, ID_PK_DH
		, ID_PK_ECDH
		, ID_PS_PK
		, ID_PS_PK_ECDH_ESCHNORR
		, ID_TA
		, ID_TA_RSA
		, ID_TA_RSA_V1_5_SHA_1
		, ID_TA_RSA_V1_5_SHA_256
		, ID_TA_RSA_PSS_SHA_1
		, ID_TA_RSA_PSS_SHA_256
		, ID_TA_RSA_V1_5_SHA_512
		, ID_TA_RSA_PSS_SHA_512
		, ID_TA_ECDSA
		, ID_TA_ECDSA_SHA_1
		, ID_TA_ECDSA_SHA_224
		, ID_TA_ECDSA_SHA_256
		, ID_TA_ECDSA_SHA_384
		, ID_TA_ECDSA_SHA_512
		, ID_CA
		, ID_CA_DH
		, ID_CA_DH_3DES_CBC_CBC
		, ID_CA_DH_AES_CBC_CMAC_128
		, ID_CA_DH_AES_CBC_CMAC_192
		, ID_CA_DH_AES_CBC_CMAC_256
		, ID_CA_ECDH
		, ID_CA_ECDH_3DES_CBC_CBC
		, ID_CA_ECDH_AES_CBC_CMAC_128
		, ID_CA_ECDH_AES_CBC_CMAC_192
		, ID_CA_ECDH_AES_CBC_CMAC_256
		, ID_PACE
		, ID_PACE_DH_GM
		, ID_PACE_DH_GM_3DES_CBC_CBC
		, ID_PACE_DH_GM_AES_CBC_CMAC_128
		, ID_PACE_DH_GM_AES_CBC_CMAC_192
		, ID_PACE_DH_GM_AES_CBC_CMAC_256
		, ID_PACE_ECDH_GM
		, ID_PACE_ECDH_GM_3DES_CBC_CBC
		, ID_PACE_ECDH_GM_AES_CBC_CMAC_128
		, ID_PACE_ECDH_GM_AES_CBC_CMAC_192
		, ID_PACE_ECDH_GM_AES_CBC_CMAC_256
		, ID_PACE_DH_IM
		, ID_PACE_DH_IM_3DES_CBC_CBC
		, ID_PACE_DH_IM_AES_CBC_CMAC_128
		, ID_PACE_DH_IM_AES_CBC_CMAC_192
		, ID_PACE_DH_IM_AES_CBC_CMAC_256
		, ID_PACE_ECDH_IM
		, ID_PACE_ECDH_IM_3DES_CBC_CBC
		, ID_PACE_ECDH_IM_AES_CBC_CMAC_128
		, ID_PACE_ECDH_IM_AES_CBC_CMAC_192
		, ID_PACE_ECDH_IM_AES_CBC_CMAC_256
		, ID_PACE_ECDH_CAM
		, ID_PACE_ECDH_CAM_AES_CBC_CMAC_128
		, ID_PACE_ECDH_CAM_AES_CBC_CMAC_192
		, ID_PACE_ECDH_CAM_AES_CBC_CMAC_256
		, ID_RI
		, ID_RI_DH
		, ID_RI_DH_SHA_1
		, ID_RI_DH_SHA_224
		, ID_RI_DH_SHA_256
		, ID_RI_DH_SHA_384
		, ID_RI_DH_SHA_512
		, ID_RI_ECDH
		, ID_RI_ECDH_SHA_1
		, ID_RI_ECDH_SHA_224
		, ID_RI_ECDH_SHA_256
		, ID_RI_ECDH_SHA_384
		, ID_RI_ECDH_SHA_512
		, ID_CI
		, ID_EID_SECURITY
		, ID_PT
		, ID_PS
		, ID_PSA
		, ID_PSA_ECDH_ECSCHNORR
		, ID_PSA_ECDH_ECSCHNORR_SHA_256
		, ID_PSA_ECDH_ECSCHNORR_SHA_384
		, ID_PSA_ECDH_ECSCHNORR_SHA_512
		, ID_PASSWORDTYPE
		, ID_PASSWORDTYPE_MRZ
		, ID_PASSWORDTYPE_CAN
		, ID_PASSWORDTYPE_PIN
		, ID_PASSWORDTYPE_PUK
		, ID_AUXILIARY_DATA
		, ID_DATE_OF_BIRTH
		, ID_DATE_OF_EXPIRY
		, ID_MUNICIPALITY_ID
		, ID_PSM_MESSAGE
		, ID_SECURITY_OBJECT
		, ID_ROLES
		, ID_IS
		, ID_AT
		, ID_ST
		, ID_EXTENSIONS
		, ID_DESCRIPTION
		, ID_PLAIN_FORMAT
		, ID_HTML_FORMAT
		, ID_PFD_FORMAT
		, ID_SECTOR
		, ID_EID_TYPE
		, ID_CARD_EID_TYPE
		, ID_MOBILE_EID_TYPE
		, ID_MOBILE_EID_TYPE_SE_CERTIFIED
		, ID_MOBILE_EID_TYPE_SE_ENDORSED
		, ID_MOBILE_EID_TYPE_HW_KEYSTORE)

class Oid
{
	friend class ::test_Oid;

	private:
		static QMap<KnownOid, int> cKnownOids;

		ASN1_OBJECT* mObject;

	public:
		static void createKnownOids();

		Oid();
		Oid(KnownOid pOid);
		explicit Oid(const ASN1_OBJECT* pObject);
		explicit Oid(const QByteArray& pTxt);
		Oid(const Oid& pOid);
		Oid(Oid&& pOid) noexcept;
		~Oid();

		Oid& operator=(const Oid& pOid);
		Oid& operator=(Oid&&) noexcept;

		bool isUndefined() const;
		QByteArray getData() const;

		explicit operator QByteArray() const;
		bool operator==(const Oid& pOther) const;
		bool operator!=(const Oid& pOther) const;
};

inline auto qHash(const Oid& pOid)
{
	return qHash(pOid.getData());
}


inline QDebug operator<<(QDebug pDbg, const Oid& pOid)
{
	QDebugStateSaver saver(pDbg);
	pDbg.noquote() << QByteArray(pOid);
	return pDbg;
}


}  // namespace governikus
