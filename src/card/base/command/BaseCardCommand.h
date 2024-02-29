/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Holds some basic card control commands
 */

#pragma once

#include "CardConnectionWorker.h"
#include "CardReturnCode.h"

#include <QSharedPointer>

class test_CardConnection;

namespace governikus
{

class BaseCardCommand
	: public QObject
{
	Q_OBJECT
	friend class ::test_CardConnection;

	private:
		Q_INVOKABLE void execute();
		QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		CardReturnCode mReturnCode;

	protected:
		explicit BaseCardCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

		[[nodiscard]] QSharedPointer<CardConnectionWorker> getCardConnectionWorker()
		{
			return mCardConnectionWorker;
		}


		void setReturnCode(CardReturnCode pReturnCode)
		{
			mReturnCode = pReturnCode;
		}


		virtual void internalExecute() = 0;
		~BaseCardCommand() override = default;

	public:
		void run();

		[[nodiscard]] CardReturnCode getReturnCode() const
		{
			return mReturnCode;
		}

	Q_SIGNALS:
		void commandDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
