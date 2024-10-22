/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Class representing a smart card
 */

#pragma once

#include "CardReturnCode.h"
#include "SmartCardDefinitions.h"
#include "apdu/CommandApdu.h"
#include "apdu/ResponseApdu.h"
#include "asn1/CVCertificateChain.h"
#include "pinpad/EstablishPaceChannelOutput.h"

#include <QObject>
#include <QPointer>


namespace governikus
{

struct TerminalAndChipAuthenticationResult
{
	CardReturnCode mReturnCode = CardReturnCode::UNDEFINED;
	QByteArray mEfCardSecurity = QByteArray();
	QByteArray mAuthenticationToken = QByteArray();
	QByteArray mNonce = QByteArray();
};

#ifndef QT_NO_DEBUG
inline bool operator ==(const TerminalAndChipAuthenticationResult& pLeft, const TerminalAndChipAuthenticationResult& pRight)
{
	return pLeft.mReturnCode == pRight.mReturnCode
		   && pLeft.mEfCardSecurity == pRight.mEfCardSecurity
		   && pLeft.mAuthenticationToken == pRight.mAuthenticationToken
		   && pLeft.mNonce == pRight.mNonce;
}


#endif

class Card
	: public QObject
{
	Q_OBJECT

	public:
		static constexpr int DEFAULT_PINPAD_TIMEOUT = 90;

		Card();
		~Card() override = default;

		/*!
		 * Establish a connection to the smart card
		 */
		virtual CardReturnCode establishConnection() = 0;

		/*!
		 * Destroys the previously established connection to the smart card
		 */
		virtual CardReturnCode releaseConnection() = 0;

		/*!
		 * Is the smart card connected, i.e. has a connection successfully been established?
		 */
		virtual bool isConnected() const = 0;

		/*!
		 * Sets the current workflow progress message. This is necessary for platforms like iOS,
		 * where interacting with a card leads to a dialog where the message needs to be updated.
		 */
		virtual void setProgressMessage(const QString& pMessage, int pProgress = -1);
		virtual void setErrorMessage(const QString& pMessage);

		/*!
		 * Performs a transmit to the smart card.
		 * The command APDU buffer is transmitted to the card.
		 * The response APDU buffer is filled with the data returned from the card.
		 */
		virtual ResponseApduResult transmit(const CommandApdu& pCmd) = 0;

		/*!
		 * Establishes a PACE channel, i.e. the corresponding reader is no basic reader.
		 */
		virtual EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription);

		/*!
		 * Destroys an existing  PACE channel, i.e. the corresponding reader is no basic reader.
		 */
		virtual CardReturnCode destroyPaceChannel();

		/*!
		 * Sets a new eID PIN, i.e. the corresponding reader is no basic reader.
		 */
		virtual ResponseApduResult setEidPin(quint8 pTimeoutSeconds);

		/*!
		 * Combines the message and progressvalue depending on the environment.
		 */
		static QString generateProgressMessage(const QString& pMessage, int pProgress = -1);
		static QString generateErrorMessage(const QString& pMessage);

		virtual EstablishPaceChannelOutput prepareIdentification(const QByteArray& pChat);

		virtual ResponseApduResult getChallenge();

		virtual TerminalAndChipAuthenticationResult performTAandCA(
			const CVCertificateChain& pTerminalCvcChain,
			const QByteArray& pAuxiliaryData,
			const QByteArray& pSignature,
			const QByteArray& pPin,
			const QByteArray& pEphemeralPublicKey);
};

} // namespace governikus
