/*!
 * \brief ASN.1 types for SecureMessaging
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "asn1/ASN1TemplateUtil.h"


namespace governikus
{

/*!
 * Message part of the Secure Messaging command/response APDU
 * containing the encrypted data.
 */
struct SM_ENCRYPTED_DATA
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_ENCRYPTED_DATA)


/*!
 * Message part of the Secure Messaging command APDU containing
 * the expected length.
 */
struct SM_PROTECTED_LE
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_PROTECTED_LE)


/*!
 * Message part of the Secure Messaging response APDU containing
 * the processing status.
 */
struct SM_PROCESSING_STATUS
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_PROCESSING_STATUS)


/*!
 * Message part of the Secure Messaging command/response APDU
 * containing the cryptographic checksum.
 */
struct SM_CHECKSUM
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_CHECKSUM)


}  // namespace governikus
