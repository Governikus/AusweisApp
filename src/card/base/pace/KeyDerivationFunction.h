/*!
 * \brief Creates key according to TR 03110 Part 3 chapters A.2.3 ff.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QCryptographicHash>
#include <QString>

namespace governikus
{

class KeyDerivationFunction final
{
	private:
		QCryptographicHash::Algorithm mHashAlgorithm;
		int mKeySize;
		QByteArray deriveKey(const QByteArray& pK, const QByteArray& pNonce, quint32 pC);

	public:
		/*!
		 * \brief Creates a new instance with derivation function algorithm determined by parameter.
		 * \param pPaceAlgorithm algorithm of PACE protocol. This will determine the key derivation algorithm to use. E.g. a
		 *        PACE protocol of id_PACE::DH::GM_AES_CBC_CMAC_128 will result in SHA256 to be used internally to derive keys.
		 */
		KeyDerivationFunction(const QByteArray& pPaceAlgorithm);
		~KeyDerivationFunction() = default;

		/*!
		 * Returns true, if initialization succeeded, i.e. the algorithm is known, supported and the key bytes have correct size.
		 */
		bool isInitialized();

		/*!
		 * \brief Derive the encryption key
		 * \param pSecret the secret to use.
		 * \return the encryption key
		 */
		QByteArray enc(const QByteArray& pSecret);

		/*!
		 * \brief Derive the MAC key
		 * \param pSecret the secret to use.
		 * \return the MAC key
		 */
		QByteArray mac(const QByteArray& pSecret);

		/*!
		 * \brief Derive the password key
		 * \param pSecret the secret to use.
		 * \return the password key
		 */
		QByteArray pi(const QString& pSecret);

};

} // namespace governikus
