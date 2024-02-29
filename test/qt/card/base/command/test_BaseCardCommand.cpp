/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref BaseCardCommand
 */

#include "command/BaseCardCommand.h"

#include "LogHandler.h"
#include "MockCardConnectionWorker.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class BaseCardCommandDummy
	: public BaseCardCommand
{
	Q_OBJECT

	public:
		BaseCardCommandDummy(Reader* pReader)
			: BaseCardCommand(CardConnectionWorker::create(pReader))
		{
		}


		void internalExecute() override
		{
			setReturnCode(CardReturnCode::OK);
		}


};

class test_BaseCardCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void commandDone()
		{
			MockReader reader("dummy reader"_L1);
			BaseCardCommandDummy command(&reader);
			QCOMPARE(command.getReturnCode(), CardReturnCode::UNKNOWN);

			QSignalSpy spy(&command, &BaseCardCommand::commandDone);
			command.run();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			auto param = spy.takeFirst();
			QSharedPointer<BaseCardCommand> sharedCommand = param.at(0).value<QSharedPointer<BaseCardCommand>>();
			QCOMPARE(sharedCommand.data(), &command);
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


};

QTEST_GUILESS_MAIN(test_BaseCardCommand)
#include "test_BaseCardCommand.moc"
