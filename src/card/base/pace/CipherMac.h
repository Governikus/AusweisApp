/*!
 * \brief CMAC implementation to be used in PACE protocol. See TR 03110.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SecurityProtocol.h"

#include <QByteArray>
#include <openssl/evp.h>

namespace governikus
{

class CipherMac final
{
	Q_DISABLE_COPY(CipherMac)

	private:
#if OPENSSL_VERSION_NUMBER < 0x30000000L
		EVP_PKEY * mKey;

#else
		EVP_MAC* mMac;
		EVP_MAC_CTX* mCtx;
#endif

	public:
		/*!
		 * \brief Creates a new instance with cipher algorithm determined by parameter and specified MAC key.
		 * \param pSecurityProtocol will determine the cipher algorithm to use. E.g. a PACE protocol
		 *        of id_PACE::DH::GM_AES_CBC_CMAC_128 will result in AES to be used for CMAC.
		 * \param pKeyBytes the bytes of the key
		 */
		CipherMac(const SecurityProtocol& pSecurityProtocol, const QByteArray& pKeyBytes);
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
