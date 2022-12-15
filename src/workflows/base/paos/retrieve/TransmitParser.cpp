/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TransmitParser.h"

#include <QDebug>

using namespace governikus;

TransmitParser::TransmitParser()
	: PaosParser(QStringLiteral("Transmit"))
{
}


PaosMessage* TransmitParser::parseMessage()
{
	mTransmit.reset(new Transmit());

	QString slotHandle;

	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("SlotHandle"))
		{
			if (readUniqueElementText(slotHandle))
			{
				mTransmit->setSlotHandle(slotHandle);
			}
		}
		else if (name == QLatin1String("InputAPDUInfo"))
		{
			parseInputApduInfo();
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	return parserFailed() ? nullptr : mTransmit.release();
}


void TransmitParser::parseSlotHandle()
{
}


void TransmitParser::parseInputApduInfo()
{
	InputAPDUInfo inputApduInfo;

	QString inputApdu;

	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("InputAPDU"))
		{
			if (!readUniqueElementText(inputApdu))
			{
				return;
			}
		}
		else if (name == QLatin1String("AcceptableStatusCode"))
		{
			inputApduInfo.addAcceptableStatusCode(readElementText().toLatin1());
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	if (inputApdu.isNull())
	{
		qCWarning(paos) << "InputAPDU element missing";
		setParserFailed();
		return;
	}

	inputApduInfo.setInputApdu(QByteArray::fromHex(inputApdu.toUtf8()));

	mTransmit->appendInputApduInfo(inputApduInfo);
}
