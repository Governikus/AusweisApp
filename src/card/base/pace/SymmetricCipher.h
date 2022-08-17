/*!
 * \brief Symmetric decryption method used for PACE.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SecurityProtocol.h"

#include <QByteArray>
#include <openssl/evp.h>

namespace governikus
{

class SymmetricCipher final
{
	Q_DISABLE_COPY(SymmetricCipher)

	private:
		EVP_CIPHER_CTX* mCtx;
		const EVP_CIPHER* mCipher;
		QByteArray mIv;
		QByteArray mKeyBytes;

	public:
		/*!
		 * \brief Creates a new instance with cipher algorithm determined by parameter and specified cipher key.
		 * \param pSecurityProtocol will determine the cipher algorithm to use. E.g. a PACE protocol
		 *        of id_PACE::DH::GM_AES_CBC_CMAC_128 will result in AES to be used.
		 * \param pKeyBytes the bytes of the key
		 */
		SymmetricCipher(const SecurityProtocol& pSecurityProtocol, const QByteArray& pKeyBytes);
		~SymmetricCipher();

		/*!
		 * Returns true, if initialization succeeded, i.e. the algorithm is known, supported and the key bytes have correct size.
		 */
		[[nodiscard]] bool isInitialized() const;

		/*!
		 * \brief Encrypts the message.
		 * \param pPlainData the message to encrypt.
		 * \return the encrypted message
		 */
		QByteArray encrypt(const QByteArray& pPlainData);

		/*!
		 * \brief Decrypts the message.
		 * \param pEncryptedData the message to decrypt.
		 * \return the decrypted message
		 */
		QByteArray decrypt(const QByteArray& pEncryptedData);

		/*!
		 * \brief Sets the initialization vector
		 * \param pIv the initialization vector
		 * \return if initialization vector has wrong size, false is returned. Otherwise true.
		 */
		bool setIv(const QByteArray& pIv);

		[[nodiscard]] int getBlockSize() const;
};

} // namespace governikus
