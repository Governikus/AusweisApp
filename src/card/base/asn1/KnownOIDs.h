/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QByteArray>

//
//  Note: It may be a good idea to keep the namespace indentation here.
//

namespace governikus
{
namespace KnownOIDs
{

const QByteArray bsi_de(QByteArrayLiteral("0.4.0.127.0.7"));

const QByteArray signedData(QByteArrayLiteral("1.2.840.113549.1.7.2"));

const QByteArray id_SecurityObject(QByteArrayLiteral("0.4.0.127.0.7.3.2.1"));

const QByteArray id_extensions = bsi_de + QByteArrayLiteral(".3.1.3");


namespace CertificateExtensions
{
const QByteArray id_description = id_extensions + ".1";

const QByteArray id_sector = id_extensions + ".2";
}

namespace TermsOfUsageType
{

/**
 * OID for the terms of usage type plain text.
 */
const QByteArray id_plainFormat = CertificateExtensions::id_description + ".1";

/**
 * OID for the terms of usage type html.
 */
const QByteArray id_htmlFormat = CertificateExtensions::id_description + ".2";

/**
 * OID for the terms of usage type pdf.
 */
const QByteArray id_pdfFormat = CertificateExtensions::id_description + ".3";
}

namespace CHATType
{
/**
 * OID for the CHAT type inspection systems.
 */
const QByteArray id_IS = bsi_de + ".3.1.2.1";

/**
 * OID for the CHAT type authentication terminals.
 */
const QByteArray id_AT = bsi_de + ".3.1.2.2";

/**
 * OID for the CHAT type signature terminals.
 */
const QByteArray id_ST = bsi_de + ".3.1.2.3";
}

namespace AuxilaryData
{

const QByteArray id_DateOfBirth = bsi_de + ".3.1.4.1";

const QByteArray id_DateOfExpiry = bsi_de + ".3.1.4.2";

const QByteArray id_CommunityID = bsi_de + ".3.1.4.3";

}

namespace SecurityProtocol
{
const QByteArray id_CA = bsi_de + ".2.2.3";
const QByteArray id_PACE = bsi_de + ".2.2.4";
const QByteArray id_TA = bsi_de + ".2.2.2";
const QByteArray id_PK = bsi_de + ".2.2.1";
}

namespace id_CA
{
const QByteArray DH = SecurityProtocol::id_CA + ".1";
const QByteArray DH_3DES_CBC_CBC = SecurityProtocol::id_CA + ".1.1";
const QByteArray DH_AES_CBC_CMAC_128 = SecurityProtocol::id_CA + ".1.2";
const QByteArray DH_AES_CBC_CMAC_192 = SecurityProtocol::id_CA + ".1.3";
const QByteArray DH_AES_CBC_CMAC_256 = SecurityProtocol::id_CA + ".1.4";
const QByteArray ECDH = SecurityProtocol::id_CA + ".2";
const QByteArray ECDH_3DES_CBC_CBC = SecurityProtocol::id_CA + ".2.1";
const QByteArray ECDH_AES_CBC_CMAC_128 = SecurityProtocol::id_CA + ".2.2";
const QByteArray ECDH_AES_CBC_CMAC_192 = SecurityProtocol::id_CA + ".2.3";
const QByteArray ECDH_AES_CBC_CMAC_256 = SecurityProtocol::id_CA + ".2.4";
}

namespace id_TA
{
const QByteArray ECDSA_SHA_1 = SecurityProtocol::id_TA + ".2.1";
const QByteArray ECDSA_SHA_224 = SecurityProtocol::id_TA + ".2.2";
const QByteArray ECDSA_SHA_256 = SecurityProtocol::id_TA + ".2.3";
const QByteArray ECDSA_SHA_384 = SecurityProtocol::id_TA + ".2.4";
const QByteArray ECDSA_SHA_512 = SecurityProtocol::id_TA + ".2.5";
}

namespace id_PK
{
const QByteArray DH = SecurityProtocol::id_PK + ".1";
const QByteArray ECDH = SecurityProtocol::id_PK + ".2";
}

namespace id_PACE
{
namespace DH
{
const QByteArray GM = SecurityProtocol::id_PACE + ".1";
const QByteArray GM_3DES_CBC_CBC = GM + ".1";
const QByteArray GM_AES_CBC_CMAC_128 = GM + ".2";
const QByteArray GM_AES_CBC_CMAC_192 = GM + ".3";
const QByteArray GM_AES_CBC_CMAC_256 = GM + ".4";

const QByteArray IM = SecurityProtocol::id_PACE + ".3";
const QByteArray IM_3DES_CBC_CBC = IM + ".1";
const QByteArray IM_AES_CBC_CMAC_128 = IM + ".2";
const QByteArray IM_AES_CBC_CMAC_192 = IM + ".3";
const QByteArray IM_AES_CBC_CMAC_256 = IM + ".4";
}

namespace ECDH
{
const QByteArray GM = SecurityProtocol::id_PACE + ".2";
const QByteArray GM_3DES_CBC_CBC = GM + ".1";
const QByteArray GM_AES_CBC_CMAC_128 = GM + ".2";
const QByteArray GM_AES_CBC_CMAC_192 = GM + ".3";
const QByteArray GM_AES_CBC_CMAC_256 = GM + ".4";

const QByteArray IM = SecurityProtocol::id_PACE + ".4";
const QByteArray IM_3DES_CBC_CBC = IM + ".1";
const QByteArray IM_AES_CBC_CMAC_128 = IM + ".2";
const QByteArray IM_AES_CBC_CMAC_192 = IM + ".3";
const QByteArray IM_AES_CBC_CMAC_256 = IM + ".4";
}
}
}
}
