/*!
 * \brief Command to perform the DID Authenticate EAC1 process.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/Chat.h"
#include "BaseCardCommand.h"

class test_DidAuthenticateEAC1Command;
class test_StateDidAuthenticateEac1;

namespace governikus
{

class DidAuthenticateEAC1Command
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_DidAuthenticateEAC1Command;
		friend class ::test_StateDidAuthenticateEac1;
		QByteArray mChallenge;

	protected:
		void internalExecute() override;
		~DidAuthenticateEAC1Command() override = default;

	public:
		explicit DidAuthenticateEAC1Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

		[[nodiscard]] const QByteArray& getChallenge() const
		{
			return mChallenge;
		}


};

} // namespace governikus
