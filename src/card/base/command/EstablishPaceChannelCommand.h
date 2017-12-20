/*!
 * \brief Command to change the PIN of a card.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
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
		const PACE_PASSWORD_ID mPacePasswordId;
		const QString mPacePassword;
		const QByteArray mEffectiveChat;
		const QByteArray mCertificateDescription;
		EstablishPACEChannelOutput mPaceOutput;

	protected:
		virtual void internalExecute() override;
		virtual ~EstablishPaceChannelCommand() override;

	public:
		EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				PACE_PASSWORD_ID pPacePasswordId, const QString& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription);

		const EstablishPACEChannelOutput& getPaceOutput() const;
};

} /* namespace governikus */
