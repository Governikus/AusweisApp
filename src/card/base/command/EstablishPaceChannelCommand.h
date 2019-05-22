/*!
 * \brief Command to change the PIN of a card.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_EstablishPaceChannelCommand;

namespace governikus
{

class EstablishPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_EstablishPaceChannelCommand;

		const PacePasswordId mPacePasswordId;
		const QString mPacePassword;
		const QByteArray mEffectiveChat;
		const QByteArray mCertificateDescription;
		EstablishPaceChannelOutput mPaceOutput;

	protected:
		virtual void internalExecute() override;
		virtual ~EstablishPaceChannelCommand() override = default;

	public:
		EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				PacePasswordId pPacePasswordId, const QString& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription);

		const EstablishPaceChannelOutput& getPaceOutput() const;
};

} // namespace governikus
