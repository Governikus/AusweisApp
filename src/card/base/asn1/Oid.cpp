/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Oid.h"

#include "EnumHelper.h"
#include "Initializer.h"

#include <openssl/asn1.h>


using namespace governikus;


INIT_FUNCTION(Oid::createKnownOids)


QMap<KnownOid, int> Oid::cKnownOids;


void Oid::createKnownOids()
{
	if (!cKnownOids.isEmpty())
	{
		return;
	}

	cKnownOids[KnownOid::ID_SIGNED_DATA] = OBJ_sn2nid("pkcs7-signedData");

	cKnownOids[KnownOid::EC_PS_PUBLICKEY] = OBJ_create("0.4.0.127.0.7.1.1.2.3", "ecPSPublicKey", "BSI TR-03110 Part 3 Version 2.21 Static keys for Pseudonymous Signatures A.6.1");

	cKnownOids[KnownOid::STANDARDIZED_DOMAINPARAMETERS] = OBJ_create("0.4.0.127.0.7.1.2", "standardizedDomainParameters", "BSI TR-03110 Part 3 Version 2.21 Standardized Domain Parameters A.2.1.1");

	cKnownOids[KnownOid::BSI_DE_PROTOCOLS_SMARTCARD] = OBJ_create("0.4.0.127.0.7.2.2", "bsi-de-protocols-smartcard", "BSI TR-03110 Part 3 Version 2.21 Supported Protocols A.1.1");

	cKnownOids[KnownOid::ID_PK] = OBJ_create("0.4.0.127.0.7.2.2.1", "id-PK", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1");
	cKnownOids[KnownOid::ID_PK_DH] = OBJ_create("0.4.0.127.0.7.2.2.1.1", "id-PK-DH", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.1");
	cKnownOids[KnownOid::ID_PK_ECDH] = OBJ_create("0.4.0.127.0.7.2.2.1.2", "id-PK-ECDH", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.2");
	cKnownOids[KnownOid::ID_PS_PK] = OBJ_create("0.4.0.127.0.7.2.2.1.3", "id-PS-PK", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.3");
	cKnownOids[KnownOid::ID_PS_PK_ECDH_ESCHNORR] = OBJ_create("0.4.0.127.0.7.2.2.1.3.2", "id-PS-PK-ECDH-ECSchnorr", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.3.2");

	cKnownOids[KnownOid::ID_TA] = OBJ_create("0.4.0.127.0.7.2.2.2", "id-TA", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3");
	cKnownOids[KnownOid::ID_TA_RSA] = OBJ_create("0.4.0.127.0.7.2.2.2.1", "id-TA-RSA", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1");
	cKnownOids[KnownOid::ID_TA_RSA_V1_5_SHA_1] = OBJ_create("0.4.0.127.0.7.2.2.2.1.1", "id-TA-RSA-v1-5-SHA-1", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.1");
	cKnownOids[KnownOid::ID_TA_RSA_V1_5_SHA_256] = OBJ_create("0.4.0.127.0.7.2.2.2.1.2", "id-TA-RSA-v1-5-SHA-256", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.2");
	cKnownOids[KnownOid::ID_TA_RSA_PSS_SHA_1] = OBJ_create("0.4.0.127.0.7.2.2.2.1.3", "id-TA-RSA-PSS-SHA-1", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.3");
	cKnownOids[KnownOid::ID_TA_RSA_PSS_SHA_256] = OBJ_create("0.4.0.127.0.7.2.2.2.1.4", "id-TA-RSA-PSS-SHA-256", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.4");
	cKnownOids[KnownOid::ID_TA_RSA_V1_5_SHA_512] = OBJ_create("0.4.0.127.0.7.2.2.2.1.5", "id-TA-RSA-v1-5-SHA-512", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.5");
	cKnownOids[KnownOid::ID_TA_RSA_PSS_SHA_512] = OBJ_create("0.4.0.127.0.7.2.2.2.1.6", "id-TA-RSA-PSS-SHA-512", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.6");
	cKnownOids[KnownOid::ID_TA_ECDSA] = OBJ_create("0.4.0.127.0.7.2.2.2.2", "id-TA-ECDSA", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2");
	cKnownOids[KnownOid::ID_TA_ECDSA_SHA_1] = OBJ_create("0.4.0.127.0.7.2.2.2.2.1", "id-TA-ECDSA-SHA-1", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.1");
	cKnownOids[KnownOid::ID_TA_ECDSA_SHA_224] = OBJ_create("0.4.0.127.0.7.2.2.2.2.2", "id-TA-ECDSA-SHA-224", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.2");
	cKnownOids[KnownOid::ID_TA_ECDSA_SHA_256] = OBJ_create("0.4.0.127.0.7.2.2.2.2.3", "id-TA-ECDSA-SHA-256", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.3");
	cKnownOids[KnownOid::ID_TA_ECDSA_SHA_384] = OBJ_create("0.4.0.127.0.7.2.2.2.2.4", "id-TA-ECDSA-SHA-384", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.4");
	cKnownOids[KnownOid::ID_TA_ECDSA_SHA_512] = OBJ_create("0.4.0.127.0.7.2.2.2.2.5", "id-TA-ECDSA-SHA-512", "BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.5");

	cKnownOids[KnownOid::ID_CA] = OBJ_create("0.4.0.127.0.7.2.2.3", "id-CA", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3");
	cKnownOids[KnownOid::ID_CA_DH] = OBJ_create("0.4.0.127.0.7.2.2.3.1", "id-CA-DH", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1");
	cKnownOids[KnownOid::ID_CA_DH_3DES_CBC_CBC] = OBJ_create("0.4.0.127.0.7.2.2.3.1.1", "id-CA-DH-3DES-CBC-CBC", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.1");
	cKnownOids[KnownOid::ID_CA_DH_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.3.1.2", "id-CA-DH-AES-CBC-CMAC-128", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.2");
	cKnownOids[KnownOid::ID_CA_DH_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.3.1.3", "id-CA-DH-AES-CBC-CMAC-192", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.3");
	cKnownOids[KnownOid::ID_CA_DH_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.3.1.4", "id-CA-DH-AES-CBC-CMAC-256", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.4");
	cKnownOids[KnownOid::ID_CA_ECDH] = OBJ_create("0.4.0.127.0.7.2.2.3.2", "id-CA-ECDH", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2");
	cKnownOids[KnownOid::ID_CA_ECDH_3DES_CBC_CBC] = OBJ_create("0.4.0.127.0.7.2.2.3.2.1", "id-CA-ECDH-3DES-CBC-CBC", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.1");
	cKnownOids[KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.3.2.2", "id-CA-ECDH-AES-CBC-CMAC-128", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.2");
	cKnownOids[KnownOid::ID_CA_ECDH_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.3.2.3", "id-CA-ECDH-AES-CBC-CMAC-192", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.3");
	cKnownOids[KnownOid::ID_CA_ECDH_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.3.2.4", "id-CA-ECDH-AES-CBC-CMAC-256", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.4");

	cKnownOids[KnownOid::ID_PACE] = OBJ_create("0.4.0.127.0.7.2.2.4", "id-PACE", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4");
	cKnownOids[KnownOid::ID_PACE_DH_GM] = OBJ_create("0.4.0.127.0.7.2.2.4.1", "id-PACE-DH-GM", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1");
	cKnownOids[KnownOid::ID_PACE_DH_GM_3DES_CBC_CBC] = OBJ_create("0.4.0.127.0.7.2.2.4.1.1", "id-PACE-DH-GM-3DES-CBC-CBC", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.1");
	cKnownOids[KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.4.1.2", "id-PACE-DH-GM-AES-CBC-CMAC-128", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.2");
	cKnownOids[KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.4.1.3", "id-PACE-DH-GM-AES-CBC-CMAC-192", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.3");
	cKnownOids[KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.4.1.4", "id-PACE-DH-GM-AES-CBC-CMAC-256", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.4");
	cKnownOids[KnownOid::ID_PACE_ECDH_GM] = OBJ_create("0.4.0.127.0.7.2.2.4.2", "id-PACE-ECDH-GM", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2");
	cKnownOids[KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC] = OBJ_create("0.4.0.127.0.7.2.2.4.2.1", "id-PACE-ECDH-GM-3DES-CBC-CBC", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.1");
	cKnownOids[KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.4.2.2", "id-PACE-ECDH-GM-AES-CBC-CMAC-128", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.2");
	cKnownOids[KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.4.2.3", "id-PACE-ECDH-GM-AES-CBC-CMAC-192", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.3");
	cKnownOids[KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.4.2.4", "id-PACE-ECDH-GM-AES-CBC-CMAC-256", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.4");
	cKnownOids[KnownOid::ID_PACE_DH_IM] = OBJ_create("0.4.0.127.0.7.2.2.4.3", "id-PACE-DH-IM", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3");
	cKnownOids[KnownOid::ID_PACE_DH_IM_3DES_CBC_CBC] = OBJ_create("0.4.0.127.0.7.2.2.4.3.1", "id-PACE-DH-IM-3DES-CBC-CBC", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.1");
	cKnownOids[KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.4.3.2", "id-PACE-DH-IM-AES-CBC-CMAC-128", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.2");
	cKnownOids[KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.4.3.3", "id-PACE-DH-IM-AES-CBC-CMAC-192", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.3");
	cKnownOids[KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.4.3.4", "id-PACE-DH-IM-AES-CBC-CMAC-256", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.4");
	cKnownOids[KnownOid::ID_PACE_ECDH_IM] = OBJ_create("0.4.0.127.0.7.2.2.4.4", "id-PACE-ECDH-IM", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4");
	cKnownOids[KnownOid::ID_PACE_ECDH_IM_3DES_CBC_CBC] = OBJ_create("0.4.0.127.0.7.2.2.4.4.1", "id-PACE-ECDH-IM-3DES-CBC-CBC", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.1");
	cKnownOids[KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.4.4.2", "id-PACE-ECDH-IM-AES-CBC-CMAC-128", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.2");
	cKnownOids[KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.4.4.3", "id-PACE-ECDH-IM-AES-CBC-CMAC-192", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.3");
	cKnownOids[KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.4.4.4", "id-PACE-ECDH-IM-AES-CBC-CMAC-256", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.4");
	cKnownOids[KnownOid::ID_PACE_ECDH_CAM] = OBJ_create("0.4.0.127.0.7.2.2.4.6", "id-PACE-ECDH-CAM", "BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6");
	cKnownOids[KnownOid::ID_PACE_ECDH_CAM_AES_CBC_CMAC_128] = OBJ_create("0.4.0.127.0.7.2.2.4.6.2", "id-PACE-ECDH-CAM-AES-CBC-CMAC-128", "BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6.2");
	cKnownOids[KnownOid::ID_PACE_ECDH_CAM_AES_CBC_CMAC_192] = OBJ_create("0.4.0.127.0.7.2.2.4.6.3", "id-PACE-ECDH-CAM-AES-CBC-CMAC-192", "BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6.3");
	cKnownOids[KnownOid::ID_PACE_ECDH_CAM_AES_CBC_CMAC_256] = OBJ_create("0.4.0.127.0.7.2.2.4.6.4", "id-PACE-ECDH-CAM-AES-CBC-CMAC-256", "BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6.4");

	cKnownOids[KnownOid::ID_RI] = OBJ_create("0.4.0.127.0.7.2.2.5", "id-RI", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4");
	cKnownOids[KnownOid::ID_RI_DH] = OBJ_create("0.4.0.127.0.7.2.2.5.1", "id-RI-DH", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1");
	cKnownOids[KnownOid::ID_RI_DH_SHA_1] = OBJ_create("0.4.0.127.0.7.2.2.5.1.1", "id-RI-DH-SHA-1", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.1");
	cKnownOids[KnownOid::ID_RI_DH_SHA_224] = OBJ_create("0.4.0.127.0.7.2.2.5.1.2", "id-RI-DH-SHA-224", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.2");
	cKnownOids[KnownOid::ID_RI_DH_SHA_256] = OBJ_create("0.4.0.127.0.7.2.2.5.1.3", "id-RI-DH-SHA-256", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.3");
	cKnownOids[KnownOid::ID_RI_DH_SHA_384] = OBJ_create("0.4.0.127.0.7.2.2.5.1.4", "id-RI-DH-SHA-384", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.4");
	cKnownOids[KnownOid::ID_RI_DH_SHA_512] = OBJ_create("0.4.0.127.0.7.2.2.5.1.5", "id-RI-DH-SHA-512", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.5");
	cKnownOids[KnownOid::ID_RI_ECDH] = OBJ_create("0.4.0.127.0.7.2.2.5.2", "id-RI-ECDH", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2");
	cKnownOids[KnownOid::ID_RI_ECDH_SHA_1] = OBJ_create("0.4.0.127.0.7.2.2.5.2.1", "id-RI-ECDH-SHA-1", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.1");
	cKnownOids[KnownOid::ID_RI_ECDH_SHA_224] = OBJ_create("0.4.0.127.0.7.2.2.5.2.2", "id-RI-ECDH-SHA-224", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.2");
	cKnownOids[KnownOid::ID_RI_ECDH_SHA_256] = OBJ_create("0.4.0.127.0.7.2.2.5.2.3", "id-RI-ECDH-SHA-256", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.3");
	cKnownOids[KnownOid::ID_RI_ECDH_SHA_384] = OBJ_create("0.4.0.127.0.7.2.2.5.2.4", "id-RI-ECDH-SHA-384", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.4");
	cKnownOids[KnownOid::ID_RI_ECDH_SHA_512] = OBJ_create("0.4.0.127.0.7.2.2.5.2.5", "id-RI-ECDH-SHA-512", "BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.5");

	cKnownOids[KnownOid::ID_CI] = OBJ_create("0.4.0.127.0.7.2.2.6", "id-CI", "BSI TR-03110 Part 3 Version 2.21 CardInfo (eIDAS token only) A.1.1.7");

	cKnownOids[KnownOid::ID_EID_SECURITY] = OBJ_create("0.4.0.127.0.7.2.2.7", "id-eIDSecurity", "BSI TR-03110 Part 3 Version 2.21 EIDSecurityInfo (eIDAS token only) A.1.1.8");

	cKnownOids[KnownOid::ID_PT] = OBJ_create("0.4.0.127.0.7.2.2.8", "id-PT", "BSI TR-03110 Part 3 Version 2.21 PrivilegedTerminalInfo (eIDAS token only) A.1.1.9");

	cKnownOids[KnownOid::ID_PS] = OBJ_create("0.4.0.127.0.7.2.2.11", "id-PS", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11");
	cKnownOids[KnownOid::ID_PSA] = OBJ_create("0.4.0.127.0.7.2.2.11.1", "id-PSA", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1");
	cKnownOids[KnownOid::ID_PSA_ECDH_ECSCHNORR] = OBJ_create("0.4.0.127.0.7.2.2.11.1.2", "id-PSA-ECDH-ECSchnorr", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2");
	cKnownOids[KnownOid::ID_PSA_ECDH_ECSCHNORR_SHA_256] = OBJ_create("0.4.0.127.0.7.2.2.11.1.2.3", "id-PSA-ECDH-ECSchnorr-SHA-256", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2.3");
	cKnownOids[KnownOid::ID_PSA_ECDH_ECSCHNORR_SHA_384] = OBJ_create("0.4.0.127.0.7.2.2.11.1.2.4", "id-PSA-ECDH-ECSchnorr-SHA-384", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2.4");
	cKnownOids[KnownOid::ID_PSA_ECDH_ECSCHNORR_SHA_512] = OBJ_create("0.4.0.127.0.7.2.2.11.1.2.5", "id-PSA-ECDH-ECSchnorr-SHA-512", "BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2.5");

	cKnownOids[KnownOid::ID_PASSWORDTYPE] = OBJ_create("0.4.0.127.0.7.2.2.12", "id-PasswordType", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12");
	cKnownOids[KnownOid::ID_PASSWORDTYPE_MRZ] = OBJ_create("0.4.0.127.0.7.2.2.12.1", "id-PasswordType-MRZ", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.1");
	cKnownOids[KnownOid::ID_PASSWORDTYPE_CAN] = OBJ_create("0.4.0.127.0.7.2.2.12.2", "id-PasswordType-CAN", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.2");
	cKnownOids[KnownOid::ID_PASSWORDTYPE_PIN] = OBJ_create("0.4.0.127.0.7.2.2.12.3", "id-PasswordType-PIN", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.3");
	cKnownOids[KnownOid::ID_PASSWORDTYPE_PUK] = OBJ_create("0.4.0.127.0.7.2.2.12.4", "id-PasswordType-PUK", "BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.4");

	cKnownOids[KnownOid::ID_AUXILIARY_DATA] = OBJ_create("0.4.0.127.0.7.3.1.4", "id-AuxiliaryData", "BSI TR-03110 Part 3 Version 2.21 Object Identifier A.7.5.1");
	cKnownOids[KnownOid::ID_DATE_OF_BIRTH] = OBJ_create("0.4.0.127.0.7.3.1.4.1", "id-DateOfBirth", "BSI TR-03110 Part 3 Version 2.21 Age Verification A.7.5.2");
	cKnownOids[KnownOid::ID_DATE_OF_EXPIRY] = OBJ_create("0.4.0.127.0.7.3.1.4.2", "id-DateOfExpiry", "BSI TR-03110 Part 3 Version 2.21 Document Validity Verification A.7.5.3");
	cKnownOids[KnownOid::ID_MUNICIPALITY_ID] = OBJ_create("0.4.0.127.0.7.3.1.4.3", "id-MunicipalityID", "BSI TR-03110 Part 3 Version 2.21 Municipality ID Verification A.7.5.4");
	cKnownOids[KnownOid::ID_PSM_MESSAGE] = OBJ_create("0.4.0.127.0.7.3.1.4.4", "id-PSM-Message", "BSI TR-03110 Part 3 Version 2.21 Pseudonymous Signature of Messages A.7.5.5");

	cKnownOids[KnownOid::ID_SECURITY_OBJECT] = OBJ_create("0.4.0.127.0.7.3.2.1", "id-SecurityObject", "BSI TR-03110 Part 3 Version 2.21 Signature Format for CardSecurity and ChipSecurity A.1.2.5");

	cKnownOids[KnownOid::ID_ROLES] = OBJ_create("0.4.0.127.0.7.3.1.2", "id-roles", "BSI TR-03110 Part 4 Version 2.21 Applications and Terminals 2");
	cKnownOids[KnownOid::ID_IS] = OBJ_create("0.4.0.127.0.7.3.1.2.1", "id-IS", "BSI TR-03110 Part 4 Version 2.21 Authorization 2.1.3.2");
	cKnownOids[KnownOid::ID_AT] = OBJ_create("0.4.0.127.0.7.3.1.2.2", "id-AT", "BSI TR-03110 Part 4 Version 2.21 Authorization 2.2.3.2");
	cKnownOids[KnownOid::ID_ST] = OBJ_create("0.4.0.127.0.7.3.1.2.3", "id-ST", "BSI TR-03110 Part 3 Version 2.11 Signature Terminals C.4.3");

	cKnownOids[KnownOid::ID_EXTENSIONS] = OBJ_create("0.4.0.127.0.7.3.1.3", "id-extensions", "BSI TR-03110 Part 3 Version 2.21 Certificate Extensions for Terminal Authentication Version 2 C.3");
	cKnownOids[KnownOid::ID_DESCRIPTION] = OBJ_create("0.4.0.127.0.7.3.1.3.1", "id-description", "BSI TR-03110 Part 4 Version 2.21 Certificate Description Extension 2.2.6");
	cKnownOids[KnownOid::ID_PLAIN_FORMAT] = OBJ_create("0.4.0.127.0.7.3.1.3.1.1", "id-plainFormat", "BSI TR-03110 Part 4 Version 2.21 Plain Text Format 2.2.6.1");
	cKnownOids[KnownOid::ID_HTML_FORMAT] = OBJ_create("0.4.0.127.0.7.3.1.3.1.2", "id-htmlFormat", "BSI TR-03110 Part 4 Version 2.21 HTML Format 2.2.6.2");
	cKnownOids[KnownOid::ID_PFD_FORMAT] = OBJ_create("0.4.0.127.0.7.3.1.3.1.3", "id-pdfFormat", "BSI TR-03110 Part 4 Version 2.21 PDF Format 2.2.6.3");
	cKnownOids[KnownOid::ID_SECTOR_RI] = OBJ_create("0.4.0.127.0.7.3.1.3.2", "id-sector-ri", "BSI TR-03110 Part 3 Version 2.21 Terminal Sector for Restricted Identification C.3.2.1");
	cKnownOids[KnownOid::ID_SECTOR_PS] = OBJ_create("0.4.0.127.0.7.3.1.3.3", "id-sector-ps", "BSI TR-03110 Part 3 Version 2.21 Terminal Sector for Pseudonymous Signatures C.3.2.2");

	cKnownOids[KnownOid::ID_EID_TYPE] = OBJ_create("0.4.0.127.0.7.3.2.3", "id-eIDType", "Draft Smart-eID");
	cKnownOids[KnownOid::ID_CARD_EID_TYPE] = OBJ_create("0.4.0.127.0.7.3.2.3.1", "id-cardEIDType", "Draft Smart-eID - 1");
	cKnownOids[KnownOid::ID_MOBILE_EID_TYPE] = OBJ_create("0.4.0.127.0.7.3.2.3.2", "id-mobileEIDType", "Draft Smart-eID - 2");
	cKnownOids[KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED] = OBJ_create("0.4.0.127.0.7.3.2.3.2.1", "id-mobileEIDType-SECertified", "Draft Smart-eID - 2.1");
	cKnownOids[KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED] = OBJ_create("0.4.0.127.0.7.3.2.3.2.2", "id-mobileEIDType-SEEndorsed", "Draft Smart-eID - 2.2");
	cKnownOids[KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE] = OBJ_create("0.4.0.127.0.7.3.2.3.2.3", "id-mobileEIDType-HWKeyStore", "Draft Smart-eID - 2.3");
}


Oid::Oid()
	: mObject(nullptr)
{
}


Oid::Oid(KnownOid pOid)
	: mObject(OBJ_nid2obj(cKnownOids.value(pOid, -1)))
{
	if (!mObject)
	{
		qCritical() << "Invalid NID, was not able to create a valid OID for:" << Enum<KnownOid>::getValue(pOid);
	}
}


Oid::Oid(const ASN1_OBJECT* pObject)
	: mObject(OBJ_dup(pObject))
{
	if (!mObject)
	{
		qCritical() << "Copying failed, was not able to create a valid OID";
		return;
	}

	if (OBJ_obj2nid(mObject) == NID_undef)
	{
		qWarning() << "Unknown OID:" << *this;
	}
}


Oid::Oid(const QByteArray& pOid)
	: mObject(OBJ_txt2obj(pOid.constData(), 0))
{
	if (!mObject)
	{
		qCritical() << "Invalid text input, was not able to create a valid OID for:" << pOid;
		return;
	}

	if (OBJ_obj2nid(mObject) == NID_undef)
	{
		qWarning() << "Unknown OID:" << *this;
	}
}


Oid::Oid(const Oid& pOid)
	: mObject(OBJ_dup(pOid.mObject))
{
}


Oid::Oid(Oid&& pOid) noexcept
	: mObject(pOid.mObject)
{
	pOid.mObject = nullptr;
}


Oid::~Oid()
{
	ASN1_OBJECT_free(mObject);
}


Oid& Oid::operator=(const Oid& pOid)
{
	ASN1_OBJECT_free(mObject);
	mObject = OBJ_dup(pOid.mObject);
	return *this;
}


Oid& Oid::operator=(Oid&& pOid) noexcept
{
	ASN1_OBJECT_free(mObject);
	mObject = pOid.mObject;
	pOid.mObject = nullptr;
	return *this;
}


QByteArray Oid::getData() const
{
	if (!mObject)
	{
		return QByteArray();
	}

	const size_t len = OBJ_length(mObject);
	const uchar* const data = OBJ_get0_data(mObject);
	return QByteArray(reinterpret_cast<const char*>(data), static_cast<int>(len));
}


Oid::operator QByteArray() const
{
	if (!mObject)
	{
		return QByteArray();
	}

	const int oidSize = OBJ_obj2txt(nullptr, 0, mObject, 1);
	if (oidSize < 1)
	{
		return QByteArray();
	}

	QByteArray description(oidSize + 1, '\0'); // +1 = null termination
	OBJ_obj2txt(description.data(), static_cast<int>(description.size()), mObject, 1);
	description.resize(oidSize); // remove null termination

	if (const int nid = OBJ_obj2nid(mObject); nid != NID_undef)
	{
		description += QByteArray(" (");
		description += QByteArray(OBJ_nid2sn(nid));
		description += QByteArray(")");
	}

	return description;
}


bool Oid::isUndefined() const
{
	return mObject == nullptr || OBJ_obj2nid(mObject) == NID_undef;
}


bool Oid::operator==(const Oid& pOther) const
{
	if (mObject == pOther.mObject)
	{
		return true;
	}

	if (mObject == nullptr || pOther.mObject == nullptr)
	{
		return false;
	}

	return OBJ_cmp(mObject, pOther.mObject) == 0;
}


bool Oid::operator!=(const Oid& pOther) const
{
	return !(*this == pOther);
}


#include "moc_Oid.cpp"
