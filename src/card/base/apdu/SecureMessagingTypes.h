/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"


namespace governikus
{

/*!
 * Message part of the Secure Messaging command/response APDU
 * containing the encrypted data.
 */
struct SM_ENCRYPTED_DATA
	: public Asn1OctetStringUtil {};
DECLARE_ASN1_OBJECT(SM_ENCRYPTED_DATA)


/*!
 * Message part of the Secure Messaging command APDU containing
 * the expected length.
 */
struct SM_PROTECTED_LE
	: public Asn1OctetStringUtil {};
DECLARE_ASN1_OBJECT(SM_PROTECTED_LE)


/*!
 * Message part of the Secure Messaging response APDU containing
 * the processing status.
 */
struct SM_PROCESSING_STATUS
	: public Asn1OctetStringUtil {};
DECLARE_ASN1_OBJECT(SM_PROCESSING_STATUS)


/*!
 * Message part of the Secure Messaging command/response APDU
 * containing the cryptographic checksum.
 */
struct SM_CHECKSUM
	: public Asn1OctetStringUtil {};
DECLARE_ASN1_OBJECT(SM_CHECKSUM)


}  // namespace governikus
