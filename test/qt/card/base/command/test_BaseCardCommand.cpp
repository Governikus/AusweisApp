/*!
 * \brief Unit tests for \ref BaseCardCommand
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "command/BaseCardCommand.h"

#include "MockReader.h"

#include <QtCore>
#include <QtTest>

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


		virtual void internalExecute() override
		{
			mReturnCode = CardReturnCode::OK;
		}


};

class test_BaseCardCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void commandDone()
		{
			MockReader reader("dummy reader");
			BaseCardCommandDummy command(&reader);
			QCOMPARE(command.getReturnCode(), CardReturnCode::UNKNOWN);

			QSignalSpy spy(&command, &BaseCardCommand::commandDone);
			QMetaObject::invokeMethod(&command, "execute");

			QCOMPARE(spy.count(), 1);
			auto param = spy.takeFirst();
			QSharedPointer<BaseCardCommand> sharedCommand = param.at(0).value<QSharedPointer<BaseCardCommand> >();
			QCOMPARE(sharedCommand.data(), &command);
			QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
		}


		void checkRetryCounterAndPrepareForPaceNoCard()
		{
			MockReader reader("dummy reader");
			BaseCardCommandDummy command(&reader);
			QCOMPARE(command.checkRetryCounterAndPrepareForPace("test"), CardReturnCode::CARD_NOT_FOUND);
		}


};

QTEST_GUILESS_MAIN(test_BaseCardCommand)
#include "test_BaseCardCommand.moc"
