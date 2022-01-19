/*!
 * \brief Command to change the PIN of a card.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_EstablishPaceChannelCommand;
class MockEstablishPaceChannelCommand;

namespace governikus
{

class EstablishPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_EstablishPaceChannelCommand;
		friend class ::MockEstablishPaceChannelCommand;

		const PacePasswordId mPacePasswordId;
		const QByteArray mPacePassword;
		const QByteArray mEffectiveChat;
		const QByteArray mCertificateDescription;
		EstablishPaceChannelOutput mPaceOutput;

	protected:
		void internalExecute() override;
		~EstablishPaceChannelCommand() override = default;

	public:
		explicit EstablishPaceChannelCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				PacePasswordId pPacePasswordId, const QByteArray& pPacePassword, const QByteArray& pEffectiveChat, const QByteArray& pCertificateDescription);

		[[nodiscard]] const EstablishPaceChannelOutput& getPaceOutput() const;
};

} // namespace governikus
