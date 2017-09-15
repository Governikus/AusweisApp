/*!
 * KeyAgreement.h
 *
 * \brief the key agreement protocol use in PACE. For details see TR 03110.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "asn1/SecurityInfo.h"
#include "CardConnectionWorker.h"
#include "GeneralAuthenticateResponse.h"
#include "pace/KeyDerivationFunction.h"

#include <QSharedPointer>

namespace governikus
{


class PACEInfo;


enum class KeyAgreementStatus
{
	SUCCESS,
	FAILED,
	PROTOCOLL_ERROR
};

class KeyAgreement
{
	private:
		const QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		QByteArray mEncryptionKey;
		QByteArray mMacKey;
		QByteArray mCarCurr, mCarPrev;

		/*!
		 * \brief Determine the card's nonce. The encrypted nonce will be decrypted using the supplied PIN.
		 * This represents the first step "General Authenticate" of TR-03110 Part 3, page 47.
		 * \param pPin PIN for decryption of the nonce
		 * \return the decrypted nonce
		 */
		QByteArray determineNonce(const QString& pPin);

		/*!
		 * \brief Determines the shared secret by performing the key agreement.
		 * This represents the second and third step "General Authenticate" of TR-03110 Part 3, page 47.
		 * \param pNonce the nonce needed for key agreement.
		 * \return the shared secret between terminal and card
		 */
		virtual QByteArray determineSharedSecret(const QByteArray& pNonce) = 0;

		/*!
		 * \brief Returns the uncompressed terminal's ephemeral public key calculated during key agreement.
		 * This public key is needed for mutual authentication.
		 * \return the uncompressed terminal's ephemeral public key
		 */
		virtual QByteArray getUncompressedTerminalPublicKey() = 0;

		/*!
		 * \brief Transmit the General Authenticate (Encrypted Nonce) command to the card.
		 * \return the encrypted nonce
		 */
		QByteArray transmitGAEncryptedNonce();

		/*!
		 * \brief Performs the mutual authentication of terminal and card using the determined shared secret.
		 * This represents the forth step "General Authenticate" of TR-03110 Part 3, page 47.
		 * \return result of authentication
		 */
		KeyAgreementStatus performMutualAuthenticate();

	protected:
		const QSharedPointer<const PACEInfo> mPaceInfo;
		KeyDerivationFunction mKeyDerivationFunction;

		KeyAgreement(const QSharedPointer<const PACEInfo>& pPaceInfo, const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		/*!
		 * \brief Transmit the General Authenticate (Mapping Data) command to the card.
		 * \param pMappingData the terminal's mapping data.
		 * \return the card's mapping data
		 */
		QByteArray transmitGAMappingData(const QByteArray& pMappingData);

		/*!
		 * \brief Transmit the General Authenticate (Ephemeral Public Key) command to the card.
		 * \param pEphemeralPublicKey the terminal's ephemeral public key
		 * \return the card's ephemeral public key
		 */
		QByteArray transmitGAEphemeralPublicKey(const QByteArray& pEphemeralPublicKey);

		/*!
		 * \brief Transmit the General Authenticate (Mutual Authentication) command to the card.
		 * \param pMutualAuthenticationData the terminal's authentication token
		 * \return the complete response APDU
		 */
		QSharedPointer<GAMutualAuthenticationResponse> transmitGAMutualAuthentication(const QByteArray& pMutualAuthenticationData);

	public:
		/*!
		 * \brief Returns the uncompressed card's ephemeral public key calculated during key agreement.
		 * This public key is needed for mutual authentication.
		 * \return the uncompressed card's ephemeral public key
		 */
		virtual QByteArray getUncompressedCardPublicKey() = 0;

		/*!
		 * \brief Returns the compressed card's ephemeral public key calculated during key agreement.
		 * This public key is needed for terminal authentication.
		 * \return the compressed card's ephemeral public key
		 */
		virtual QByteArray getCompressedCardPublicKey() = 0;

		/*!
		 * \brief Factory method to create an instance of KeyAgreement.
		 * \param pPaceInfo the PACEInfo containing the protocol parameters
		 * \param pReader the reader to transmit card commands
		 * \return new instance
		 */
		static QSharedPointer<KeyAgreement> create(const QSharedPointer<const PACEInfo>& pPaceInfo,
				QSharedPointer<CardConnectionWorker> pCardConnectionWorker);
		virtual ~KeyAgreement();

		/*!
		 * \brief Perform the key agreement.
		 * \param pPin the PIN value
		 * \return the authentication result
		 */
		KeyAgreementStatus perform(const QString& pPin);

		/*!
		 * \brief Subsequent to the key agreement an encryption key is determined. This method returns this key.
		 * I. e. the output of KDF_enc according to TR-03110 Part 3 chapter A.2.3.
		 * \return the encryption key
		 */
		const QByteArray& getEncryptionKey() const;

		/*!
		 * \brief Subsequent to the key agreement a MAC key is determined. This method returns this key.
		 * I. e. the output of KDF_mac according to TR-03110 Part 3 chapter A.2.3.
		 * \return the MAC key
		 */
		const QByteArray& getMacKey() const;

		/*!
		 * \brief Subsequent to the key agreement a certificate authority reference (CAR) may be determined. This method returns this CAR.
		 * \return the card's CAR number 1
		 */
		const QByteArray& getCarCurr() const;

		/*!
		 * \brief Subsequent to the key agreement a certificate authority reference (CAR) may be determined. This method returns this CAR.
		 * \return the card's CAR number 2
		 */
		const QByteArray& getCarPrev() const;
};

} /* namespace governikus */
