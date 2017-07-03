/*!
 * Card.h
 *
 * \brief Class representing a smart card
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "Apdu.h"
#include "Commands.h"
#include "EstablishPACEChannel.h"
#include "ReturnCode.h"
#include "SmartCardDefinitions.h"

#include <QObject>
#include <QPointer>


namespace governikus
{

class Card
	: public QObject
{
	Q_OBJECT

	public:
		Card();
		virtual ~Card();

		/*!
		 * Establish a connection to the smart card
		 */
		virtual ReturnCode connect() = 0;

		/*!
		 * Destroys the previously established connection to the smart card
		 */
		virtual ReturnCode disconnect() = 0;

		/*!
		 * Is the smart card connected, i.e. has a connection successfully been established?
		 */
		virtual bool isConnected() = 0;

		/*!
		 * Performs a transmit to the smart card.
		 * The command APDU buffer is transmitted to the card.
		 * The response APDU buffer is filled with the data returned from the card.
		 */
		virtual ReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) = 0;

		/*!
		 * Establishes a PACE channel, i.e. the corresponding reader is no basic reader.
		 */
		virtual ReturnCode establishPaceChannel(PACE_PIN_ID pPinId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, int pTimeoutSeconds = 60) = 0;

		/*!
		 * Destroys an existing  PACE channel, i.e. the corresponding reader is no basic reader.
		 */
		virtual ReturnCode destroyPaceChannel();

		/*!
		 * Sets a new eID PIN, i.e. the corresponding reader is no basic reader.
		 */
		virtual ReturnCode setEidPin(unsigned int pTimeoutSeconds) = 0;

};

} /* namespace governikus */
