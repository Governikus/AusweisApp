/*!
 * \brief Implements TR-03110 v2 part3 --> Secure Messaging.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/ASN1TemplateUtil.h"
#include "CommandApdu.h"
#include "pace/CipherMac.h"
#include "pace/SymmetricCipher.h"
#include "ResponseApdu.h"

#include <QByteArray>


namespace governikus
{

/*!
 * Message part of the Secure Messaging command APDU containing
 * the expected length.
 */
struct SM_PROTECTED_LE
	: public ASN1_OCTET_STRING {};
DECLARE_ASN1_OBJECT(SM_PROTECTED_LE)


class SecureMessaging final
{
	private:
		SymmetricCipher mCipher;
		CipherMac mCipherMac;
		quint32 mSendSequenceCounter;

		QByteArray padToCipherBlockSize(const QByteArray& pData) const;
		QByteArray unpadFromCipherBlockSize(const QByteArray& pData) const;
		QByteArray createSecuredHeader(const CommandApdu& pCommandApdu) const;
		QByteArray createMac(const QByteArray& pSecuredHeader, const QByteArray& pFormattedEncryptedData, const QByteArray& pSecuredLe);
		int createNewLe(const QByteArray& pSecuredData, int pOldLe) const;
		QByteArray getSendSequenceCounter() const;
		QByteArray getEncryptedIv();

		QByteArray createSecuredLe(int pLe) const;

	public:
		SecureMessaging(const QByteArray& pPaceAlgorithm, const QByteArray& pEncKey, const QByteArray& pMacKey);
		~SecureMessaging() = default;

		/*!
		 * Returns true, if initialization succeeded, i.e. the algorithm is known, supported and the keys have correct size.
		 */
		bool isInitialized() const;

		CommandApdu encrypt(const CommandApdu& pCommandApdu);

		/*!
		 * \brief Decrypt given ResponseApdu.
		 *
		 * \return Decrypted ResponseApdu or an empty ResponseApdu if decryption fails.
		 */
		ResponseApdu decrypt(const ResponseApdu& pEncryptedResponseApdu);
};

} // namespace governikus
