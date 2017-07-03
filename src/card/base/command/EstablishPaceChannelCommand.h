/*!
 * EstablishPaceChannelCommand.h
 *
 * \brief Command to change the PIN of a card.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "BaseCardCommand.h"

namespace governikus
{

class CardConnection;

class EstablishPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		const PACE_PIN_ID mPacePinId;
		const QString mPacePin;
		const QByteArray mEffectiveChat;
		const QByteArray mCertificateDescription;
		EstablishPACEChannelOutput mPaceOutput;

	protected:
		virtual void internalExecute() override;
		virtual ~EstablishPaceChannelCommand();

	public:
		EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				PACE_PIN_ID pPacePinId, const QString& pPacePin, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription);

		const EstablishPACEChannelOutput& getPaceOutput() const;
};

} /* namespace governikus */
