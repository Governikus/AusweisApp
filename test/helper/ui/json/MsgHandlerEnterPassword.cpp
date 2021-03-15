/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterPassword.h"

#include "MockCardConnection.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QTest>

using namespace governikus;

void governikus::setValidState(MessageDispatcher& pDispatcher,
		bool pSelectReader,
		bool pBasicReader,
		const PacePasswordId pPasswordID,
		const QLatin1String& pState,
		const QSharedPointer<WorkflowContext> pContext)
{
	pDispatcher.init(pContext);
	pContext->setEstablishPaceChannelType(pPasswordID);

	QByteArray expected;
	if (pPasswordID == PacePasswordId::PACE_PIN)
	{
		expected = R"({"msg":"ENTER_PIN"})";
	}
	else if (pPasswordID == PacePasswordId::PACE_CAN)
	{
		expected = R"({"msg":"ENTER_CAN"})";
	}
	else if (pPasswordID == PacePasswordId::PACE_PUK)
	{
		expected = R"({"msg":"ENTER_PUK"})";
	}

	if (pState == QLatin1String("StateEnterNewPacePin"))
	{
		expected = R"({"msg":"ENTER_NEW_PIN"})";
	}

	QCOMPARE(pDispatcher.processStateChange(pState), expected);

	if (pSelectReader)
	{
		auto* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader CARD");
		auto info = reader->getReaderInfo();
		info.setBasicReader(pBasicReader);
		reader->setReaderInfo(info);
		reader->setCard(MockCardConfig());
		pContext->setReaderName("MockReader CARD");

		QSharedPointer<CardConnection> cardConnection(new MockCardConnection(reader->getReaderInfo()));
		pContext->setCardConnection(cardConnection);
		pContext->rememberReader();
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
