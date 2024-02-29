/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Response APDU for SecureMessaging
 */

#pragma once

#include "ResponseApdu.h"
#include "SecureMessagingApdu.h"


namespace governikus
{

/*!
 * Response APDU for Secure Messaging as defined in
 * TR-03110-3
 */
class SecureMessagingResponse
	: public SecureMessagingApdu
{
	Q_DISABLE_COPY(SecureMessagingResponse)

	private:
		QSharedPointer<SM_PROCESSING_STATUS> mProcessingStatus;
		QSharedPointer<SM_CHECKSUM> mChecksum;

	public:
		explicit SecureMessagingResponse(const ResponseApdu& pApdu);

		/*!
		 * Returns the cryptographic checsum.
		 */
		[[nodiscard]] QByteArray getMac() const;

		/*!
		 * Returns the processing status.
		 */
		[[nodiscard]] StatusCode getSecuredStatusCode() const;

		/*!
		 * Returns the processing status bytes.
		 */
		[[nodiscard]] QByteArray getSecuredStatusCodeBytes() const;

		/*!
		 * Returns the encoded ASN.1 object containing the processing status.
		 */
		[[nodiscard]] QByteArray getSecuredStatusCodeObjectEncoded() const;
};


}  // namespace governikus
