/*!
 * \brief CMAC implementation to be used in PACE protocol. See TR 03110.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <openssl/evp.h>
#include <QByteArray>

namespace governikus
{

class CipherMac final
{
	private:
		EVP_PKEY* mKey;

		Q_DISABLE_COPY(CipherMac)

	public:
		/*!
		 * \brief Creates a new instance with cipher algorithm determined by parameter and specified MAC key.
		 * \param pPaceAlgorithm algorithm of PACE protocol. This will determine the cipher algorithm to use. E.g. a
		 *        PACE protocol of id_PACE::DH::GM_AES_CBC_CMAC_128 will result in AES to be used for CMAC.
		 * \param pKeyBytes the bytes of the key
		 */
		CipherMac(const QByteArray& pPaceAlgorithm, const QByteArray& pKeyBytes);
		~CipherMac();

		/*!
		 * Returns true, if initialization succeeded, i.e. the algorithm is known, supported and the key bytes have correct size.
		 */
		[[nodiscard]] bool isInitialized() const;

		/*!
		 * \brief Generates the MAC of a message.
		 * \param pMessage the message to build the MAC for.
		 * \return the MAC of the message
		 */
		QByteArray generate(const QByteArray& pMessage);
};

} // namespace governikus
