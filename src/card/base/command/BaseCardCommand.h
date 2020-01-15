/*!
 * \brief Holds some basic card control commands
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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

	private:
		friend class ::test_CardConnection;
		Q_INVOKABLE void execute();

	protected:
		QSharedPointer<CardConnectionWorker> mCardConnectionWorker;
		CardReturnCode mReturnCode;

		explicit BaseCardCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker);

		virtual void internalExecute() = 0;
		virtual ~BaseCardCommand();

	public:
		void run();

		CardReturnCode getReturnCode() const
		{
			return mReturnCode;
		}


	Q_SIGNALS:
		void commandDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
