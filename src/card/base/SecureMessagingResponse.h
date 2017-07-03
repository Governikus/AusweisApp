/*!
 * \brief Response APDU for SecureMessaging
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "Commands.h"
#include "asn1/ASN1TemplateUtil.h"


namespace governikus
{


/*!
 * Message part of the Secure Messaging response APDU containing
 * the encrypted data.
 */
struct SM_ENCRYPTED_DATA
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_ENCRYPTED_DATA)


/*!
 * Message part of the Secure Messaging response APDU containing
 * the processing status.
 */
struct SM_PROCESSING_STATUS
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_PROCESSING_STATUS)


/*!
 * Message part of the Secure Messaging response APDU containing
 * the cryptographic checksum.
 */
struct SM_CHECKSUM
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_CHECKSUM)


/*!
 * Response APDU for Secure Messaging as defined in
 * TR-03110-3
 */
class SecureMessagingResponse
	: public ResponseApdu
{
	private:
		bool mInvalid;
		QSharedPointer<SM_ENCRYPTED_DATA> mEncryptedData;
		QSharedPointer<SM_PROCESSING_STATUS> mProcessingStatus;
		QSharedPointer<SM_CHECKSUM> mChecksum;

		Q_DISABLE_COPY(SecureMessagingResponse)

	public:
		SecureMessagingResponse(const QByteArray& pBuffer);
		virtual ~SecureMessagingResponse();

		/*!
		 * Returns the encrypted data without padding-content indicator.
		 */
		QByteArray getEncryptedData() const;

		/*!
		 * Returns the encoded ASN.1 object containing the encrypted data.
		 */
		QByteArray getEncryptedDataObjectEncoded() const;

		/*!
		 * Returns the cryptographic checsum.
		 */
		QByteArray getMac() const;

		/*!
		 * Returns the processing status.
		 */
		StatusCode getSecuredStatusCode() const;

		/*!
		 * Returns the processing status bytes.
		 */
		QByteArray getSecuredStatusCodeBytes() const;

		/*!
		 * Returns the encoded ASN.1 object containing the processing status.
		 */
		QByteArray getSecuredStatusCodeObjectEncoded() const;

		/*!
		 * Is the message structure invalid, i.e. is the message syntactically invalid?
		 */
		bool isInvalid() const;
};


}  // namespace governikus
