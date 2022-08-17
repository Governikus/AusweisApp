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
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("SlotHandle"))
		{
			if (readUniqueElementText(slotHandle))
			{
				mTransmit->setSlotHandle(slotHandle);
			}
		}
		else if (mXmlReader->name() == QLatin1String("InputAPDUInfo"))
		{
			parseInputApduInfo();
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	return mParseError ? nullptr : mTransmit.release();
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
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("InputAPDU"))
		{
			if (!readUniqueElementText(inputApdu))
			{
				return;
			}
		}
		else if (mXmlReader->name() == QLatin1String("AcceptableStatusCode"))
		{
			inputApduInfo.addAcceptableStatusCode(readElementText().toLatin1());
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	if (inputApdu.isNull())
	{
		qCWarning(paos) << "InputAPDU element missing";
		mParseError = true;
		return;
	}

	inputApduInfo.setInputApdu(QByteArray::fromHex(inputApdu.toUtf8()));

	mTransmit->appendInputApduInfo(inputApduInfo);
}
