/*!
 * \brief Holds some basic card control commands
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardConnectionWorker.h"
#include "ReturnCode.h"

#include <QSharedPointer>

class test_BaseCardCommand;

namespace governikus
{

class CardConnection;


class BaseCardCommand
	: public QObject
{
	Q_OBJECT
	friend class::test_BaseCardCommand;

	private:
		Q_INVOKABLE void execute(); // will be invoked by CardConnection

	protected:
		QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		ReturnCode mReturnCode;

		BaseCardCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

		ReturnCode checkRetryCounterAndPrepareForPace(const QString& pCan);

		virtual void internalExecute() = 0;
		virtual ~BaseCardCommand();

	public:
		ReturnCode getReturnCode() const
		{
			return mReturnCode;
		}


	Q_SIGNALS:
		void commandDone(QSharedPointer<BaseCardCommand> pCommand);
};

} /* namespace governikus */
