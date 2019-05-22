/*!
 * \brief Handler for the PACE protocol. See TR-03110.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/SecurityInfos.h"
#include "CardConnectionWorker.h"
#include "pace/KeyAgreement.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QString>

class test_PaceHandler;

namespace governikus
{
class PaceHandler final
{
	friend class ::test_PaceHandler;

	private:
		const QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		QSharedPointer<KeyAgreement> mKeyAgreement;
		QSharedPointer<const PaceInfo> mPaceInfo;
		QByteArray mStatusMseSetAt;
		QByteArray mIdIcc;
		QByteArray mEncryptionKey;
		QByteArray mMacKey;
		QByteArray mChat;
		QByteArray mCarCurr, mCarPrev;

		/*!
		 * \brief checks for implementation support
		 */
		bool isSupportedProtocol(const QSharedPointer<const PaceInfo>& pPaceInfo) const;

		/*!
		 * \brief Perform initialization of the handler. During initialization the PACE protocol parameters to be used are determined.
		 * \param pEfCardAccess the card's EFCardAccess containing all supported protocol parameters
		 * \return the initialization result
		 */
		bool initialize(const QSharedPointer<const EFCardAccess>& pEfCardAccess);

		/*!
		 * \brief Transmit the MSE:Set AT command to the card.
		 * \param pPasswordId the PACE password id to use, e.g. PIN, CAN or PUK
		 * \return false on any card errors
		 */
		CardReturnCode transmitMSESetAT(PacePasswordId pPasswordId);

		Q_DISABLE_COPY(PaceHandler)

	public:
		PaceHandler(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		/*!
		 * \brief Performs the PACE protocol and establishes a PACE channel.
		 * \param pPasswordId the PACE password id to use, e.g. PIN, CAN or PUK
		 * \param pPassword the password value, e.g. "123456"
		 * \return false on any errors during establishment
		 */
		CardReturnCode establishPaceChannel(PacePasswordId pPasswordId, const QString& pPassword);

		/*!
		 * \brief The certificate holder authorization template to be supplied to the card. May be empty
		 */
		void setChat(const QByteArray& pChat);

		/*!
		 * \brief During PACE protocol an encryption key is determined. This method returns this key.
		 * I. e. the output of KDF_enc according to TR-03110 Part 3 chapter A.2.3.
		 * \return the encryption key
		 */
		const QByteArray& getEncryptionKey() const;

		/*!
		 * \brief During PACE protocol a MAC key is determined. This method returns this key.
		 * I. e. the output of KDF_mac according to TR-03110 Part 3 chapter A.2.3.
		 * \return the MAC key
		 */
		const QByteArray& getMacKey() const;

		/*!
		 * \brief During PACE protocol a certificate authority reference (CAR) may be determined. This method returns this CAR.
		 * The CAR is determined, if a CHAT was supplied, \see setChat
		 * \return the card's CAR number 1
		 */
		const QByteArray& getCarCurr() const;

		/*!
		 * \brief During PACE protocol a certificate authority reference (CAR) may be determined. This method returns this CAR.
		 * The CAR is determined, if a CHAT was supplied, \see setChat
		 * \return the card's CAR number 2
		 */
		const QByteArray& getCarPrev() const;

		/*!
		 * \brief The IDicc is the card's compressed ephemeral public key.
		 * \return the card's compressed ephemeral public key
		 */
		const QByteArray& getIdIcc() const;

		const QByteArray& getStatusMseSetAt() const;

		/*!
		 * The used PACE protocol.
		 * \return the PACE protocol OID as string.
		 */
		QByteArray getPaceProtocol() const;
};

} // namespace governikus
