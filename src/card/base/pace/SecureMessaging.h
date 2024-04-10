/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implements TR-03110 v2 part3 --> Secure Messaging.
 */

#pragma once

#include "SecurityProtocol.h"
#include "apdu/CommandApdu.h"
#include "apdu/ResponseApdu.h"
#include "pace/CipherMac.h"
#include "pace/SymmetricCipher.h"

#include <QByteArray>


namespace governikus
{

class SecureMessaging final
{
	private:
		SymmetricCipher mCipher;
		CipherMac mCipherMac;
		quint32 mSendSequenceCounter;

		[[nodiscard]] QByteArray padToCipherBlockSize(const QByteArray& pData) const;
		[[nodiscard]] QByteArray unpadFromCipherBlockSize(const QByteArray& pData) const;
		[[nodiscard]] QByteArray createSecuredHeader(const CommandApdu& pCommandApdu) const;
		QByteArray createMac(const QByteArray& pSecuredHeader, const QByteArray& pFormattedEncryptedData, const QByteArray& pSecuredLe);
		[[nodiscard]] int createNewLe(const QByteArray& pSecuredData, int pOldLe) const;
		[[nodiscard]] QByteArray getSendSequenceCounter() const;
		QByteArray getEncryptedIv();

	public:
		SecureMessaging(const SecurityProtocol& pSecurityProtocol, const QByteArray& pEncKey, const QByteArray& pMacKey);
		~SecureMessaging() = default;

		/*!
		 * Returns true, if initialization succeeded, i.e. the algorithm is known, supported and the keys have correct size.
		 */
		[[nodiscard]] bool isInitialized() const;

		CommandApdu encrypt(const CommandApdu& pCommandApdu);

		CommandApdu decrypt(const CommandApdu& pEncryptedCommandApdu);

		ResponseApdu encrypt(const ResponseApdu& pResponseApdu);

		ResponseApdu decrypt(const ResponseApdu& pEncryptedResponseApdu);
};

} // namespace governikus
