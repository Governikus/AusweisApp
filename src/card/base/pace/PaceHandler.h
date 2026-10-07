/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "CardConnectionWorker.h"
#include "SecurityProtocol.h"
#include "asn1/SecurityInfos.h"
#include "pace/KeyAgreement.h"

#include <QByteArray>
#include <QSharedPointer>


class test_PaceHandler;


namespace governikus
{
class PaceHandler final
{
	Q_DISABLE_COPY(PaceHandler)
	friend class ::test_PaceHandler;

	private:
		const QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		QSharedPointer<KeyAgreement> mKeyAgreement;
		QSharedPointer<const PaceInfo> mPaceInfo;
		QByteArray mStatusMseSetAt;
		QByteArray mEncryptionKey;
		QByteArray mMacKey;
		QByteArray mChat;

		/*!
		 * \brief checks for implementation support
		 */
		[[nodiscard]] bool isSupportedProtocol(const QSharedPointer<const PaceInfo>& pPaceInfo) const;

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

	public:
		explicit PaceHandler(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker);

		/*!
		 * \brief Performs the PACE protocol and establishes a PACE channel.
		 * \param pPasswordId the PACE password id to use, e.g. PIN, CAN or PUK
		 * \param pPassword the password value, e.g. "123456"
		 * \return false on any errors during establishment
		 */
		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pPassword);

		/*!
		 * \brief The certificate holder authorization template to be supplied to the card. May be empty
		 */
		void setChat(const QByteArray& pChat);

		/*!
		 * \brief During PACE protocol an encryption key is determined. This method returns this key.
		 * I. e. the output of KDF_enc according to TR-03110 Part 3 chapter A.2.3.
		 * \return the encryption key
		 */
		[[nodiscard]] const QByteArray& getEncryptionKey() const;

		/*!
		 * \brief During PACE protocol a MAC key is determined. This method returns this key.
		 * I. e. the output of KDF_mac according to TR-03110 Part 3 chapter A.2.3.
		 * \return the MAC key
		 */
		[[nodiscard]] const QByteArray& getMacKey() const;


		/*!
		 * The used PACE protocol.
		 * \return the PACE specific security protocol.
		 */
		[[nodiscard]] SecurityProtocol getPaceProtocol() const;
};

} // namespace governikus
