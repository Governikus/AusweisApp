/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Worker for \ref CardConnection that will do the job in \ref ReaderManagerWorker
 */

#pragma once

#include "CardReturnCode.h"
#include "FileRef.h"
#include "Reader.h"
#include "SmartCardDefinitions.h"
#include "apdu/CommandApdu.h"
#include "apdu/ResponseApdu.h"
#include "asn1/CVCertificateChain.h"
#include "asn1/SecurityInfos.h"
#include "pace/SecureMessaging.h"
#include "pinpad/EstablishPaceChannelOutput.h"

#include <QByteArray>
#include <QTimer>


namespace governikus
{

/*!
 * This class represents a connection to a smart card.
 */
class CardConnectionWorker
	: public QObject
	, public QEnableSharedFromThis<CardConnectionWorker>
{
	Q_OBJECT

	private:
		/*!
		 * The connection talks to the Card held by the Reader
		 */
		QPointer<Reader> mReader;

		/*!
		 * Object performing the cryptography needed by a secure messaging channel
		 */
		QScopedPointer<SecureMessaging> mSecureMessaging;

		QTimer mKeepAliveTimer;

		inline QSharedPointer<const EFCardAccess> getEfCardAccess() const;

		void stopSecureMessaging();

	private Q_SLOTS:
		void onKeepAliveTimeout();

	protected:
		/*!
		 * The Card hold by the Reader is expected to be connected.
		 * The connection is closed, when the CardConnection is destroyed.
		 */
		explicit CardConnectionWorker(Reader* pReader);

		/*!
		 * Destroys the CardConnection and disconnects from the card.
		 */
		~CardConnectionWorker() override;

	public:
		static QSharedPointer<CardConnectionWorker> create(Reader* pReader);

		Q_INVOKABLE ReaderInfo getReaderInfo() const;

		void setPukInoperative();

		virtual CardReturnCode updateRetryCounter();

		virtual CardReturnCode readFile(const FileRef& pFileRef, QByteArray& pFileContent, int pLe = CommandApdu::SHORT_MAX_LE);

		virtual ResponseApduResult transmit(const CommandApdu& pCommandApdu);

		/*!
		 * Performs PACE and establishes a PACE channel for later terminal authentication.
		 * If the Reader is a basic reader and the PACE channel is successfully established, the subsequent transmits will be secured using, secure messaging.
		 * I. e., a secure messaging channel is established.
		 */
		virtual EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId,
				const QByteArray& pPasswordValue,
				const QByteArray& pChat,
				const QByteArray& pCertificateDescription);

		/*!
		 * Destroys a previously established PACE channel.
		 */
		virtual CardReturnCode destroyPaceChannel();

		virtual void setKeepAlive(bool pEnabled);

		/*!
		 * Sets the current workflow progress message. This is necessary for platforms like iOS,
		 * where interacting with a card leads to a dialog where the message needs to be updated.
		 */
		virtual void setProgressMessage(const QString& pMessage, int pProgress = -1);

		virtual ResponseApduResult setEidPin(const QByteArray& pNewPin, quint8 pTimeoutSeconds);

		EstablishPaceChannelOutput prepareIdentification(const QByteArray& pChat) const;

		ResponseApduResult getChallenge() const;

		TerminalAndChipAuthenticationResult performTAandCA(
			const CVCertificateChain& pTerminalCvcChain,
			const QByteArray& pAuxiliaryData,
			const QByteArray& pSignature,
			const QByteArray& pPin,
			const QByteArray& pEphemeralPublicKey) const;

	Q_SIGNALS:
		void fireReaderInfoChanged(const ReaderInfo& pReaderInfo);
		void fireSecureMessagingStopped();
};

} // namespace governikus
