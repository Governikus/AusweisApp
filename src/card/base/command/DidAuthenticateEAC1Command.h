/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"
#include "CardConnectionWorker.h"


class test_DidAuthenticateEAC1Command;
class test_StateDidAuthenticateEac1;


namespace governikus
{

class DidAuthenticateEAC1Command
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_DidAuthenticateEAC1Command;
	friend class ::test_StateDidAuthenticateEac1;

	private:
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
