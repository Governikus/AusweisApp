/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterPassword.h"

#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QTest>

using namespace governikus;

void governikus::setValidState(MessageDispatcher& pDispatcher,
		bool pSelectReader,
		bool pBasicReader,
		const PacePasswordId pPasswordID)
{
	QSharedPointer<WorkflowContext> context(new WorkflowContext());
	pDispatcher.init(context);
	context->setEstablishPaceChannelType(pPasswordID);

	QByteArray expected;
	if (pPasswordID == PacePasswordId::PACE_PIN)
	{
		expected = R"({"msg":"ENTER_PIN"})";
	}

	if (pPasswordID == PacePasswordId::PACE_CAN)
	{
		expected = R"({"msg":"ENTER_CAN"})";
	}

	if (pPasswordID == PacePasswordId::PACE_PUK)
	{
		expected = R"({"msg":"ENTER_PUK"})";
	}

	QCOMPARE(pDispatcher.processStateChange(QLatin1String("StateEnterPacePassword")), expected);

	if (pSelectReader)
	{
		auto* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader CARD");
		auto info = reader->getReaderInfo();
		info.setBasicReader(pBasicReader);
		reader->setReaderInfo(info);
		reader->setCard(MockCardConfig());
		context->setReaderName("MockReader CARD");
	}
}


QByteArray governikus::addReaderData(const char* pData, bool pKeyPad)
{
	const QByteArray data(pData);
	const QByteArray part1(R"(,"reader":{"attached":true,"card":{"deactivated":false,"inoperative":false,"retryCounter":-1},"keypad":)");
	const QByteArray keypad = pKeyPad ? QByteArrayLiteral("true") : QByteArrayLiteral("false");
	const QByteArray part2(R"(,"name":"MockReader CARD"})");
	return data.mid(0, data.size() - 1) + part1 + keypad + part2 + QByteArrayLiteral("}");
}
