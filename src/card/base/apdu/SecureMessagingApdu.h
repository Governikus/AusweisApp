/*!
 * \brief APDU for SecureMessaging
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SecureMessagingTypes.h"

#include <QByteArray>


namespace governikus
{

class SecureMessagingApdu
{
	private:
		bool mValid;
		QSharedPointer<SM_ENCRYPTED_DATA> mEncryptedData;

	protected:
		SecureMessagingApdu();

		bool decodeData(QByteArray& pData);

		void setValid();

	public:
		/*!
		 * Returns the encrypted data without padding-content indicator.
		 */
		[[nodiscard]] QByteArray getEncryptedData() const;

		/*!
		 * Returns the encoded ASN.1 object containing the encrypted data.
		 */
		[[nodiscard]] QByteArray getEncryptedDataObjectEncoded() const;

		/*!
		 * Is the message structure invalid, i.e. is the message syntactically invalid?
		 */
		[[nodiscard]] bool isValid() const;
};


}  // namespace governikus
