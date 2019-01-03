/*!
 * \brief Class representing a smart card
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "CommandApdu.h"
#include "Commands.h"
#include "EstablishPaceChannelOutput.h"
#include "ResponseApdu.h"
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
		virtual ~Card() = default;

		/*!
		 * Establish a connection to the smart card
		 */
		virtual CardReturnCode connect() = 0;

		/*!
		 * Destroys the previously established connection to the smart card
		 */
		virtual CardReturnCode disconnect() = 0;

		/*!
		 * Is the smart card connected, i.e. has a connection successfully been established?
		 */
		virtual bool isConnected() = 0;

		/*!
		 * Performs a transmit to the smart card.
		 * The command APDU buffer is transmitted to the card.
		 * The response APDU buffer is filled with the data returned from the card.
		 */
		virtual CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) = 0;

		/*!
		 * Establishes a PACE channel, i.e. the corresponding reader is no basic reader.
		 */
		virtual CardReturnCode establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPaceChannelOutput& pChannelOutput, quint8 pTimeoutSeconds = 60);

		/*!
		 * Destroys an existing  PACE channel, i.e. the corresponding reader is no basic reader.
		 */
		virtual CardReturnCode destroyPaceChannel();

		/*!
		 * Sets a new eID PIN, i.e. the corresponding reader is no basic reader.
		 */
		virtual CardReturnCode setEidPin(quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu);

};

} // namespace governikus
