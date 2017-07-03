/*!
 * DidAuthenticateEAC1Command.h
 *
 * \brief Command to perform the DID Authenticate EAC1 process.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BaseCardCommand.h"
#include "Commands.h"
#include "asn1/Chat.h"

namespace governikus
{

class CardConnection;

class DidAuthenticateEAC1Command
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		QByteArray mChallenge;

	protected:
		virtual void internalExecute() override;
		virtual ~DidAuthenticateEAC1Command();

	public:
		DidAuthenticateEAC1Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);


		const QByteArray& getChallenge() const
		{
			return mChallenge;
		}


};

} /* namespace governikus */
