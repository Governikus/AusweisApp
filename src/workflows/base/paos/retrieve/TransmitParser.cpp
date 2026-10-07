/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "TransmitParser.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


void TransmitParser::parseInputApduInfo(Transmit& pTransmit)
{
	InputAPDUInfo inputApduInfo;
	QString inputApdu;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("InputAPDU"))
		{
			if (!mParser->readUniqueElementText(inputApdu))
			{
				return;
			}
		}
		else if (name == QLatin1String("AcceptableStatusCode"))
		{
			inputApduInfo.addAcceptableStatusCode(mParser->readElementText().toLatin1());
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			mParser->skipCurrentElement();
		}
	}

	if (inputApdu.isNull())
	{
		qCWarning(paos) << "InputAPDU element missing";
		mParser->setParserFailed();
		return;
	}

	inputApduInfo.setInputApdu(QByteArray::fromHex(inputApdu.toUtf8()));

	pTransmit.appendInputApduInfo(inputApduInfo);
}


TransmitParser::TransmitParser(const QSharedPointer<ElementParser>& pParser)
	: mParser(pParser)
{
}


TransmitParser::~TransmitParser() = default;

std::unique_ptr<Transmit> TransmitParser::parse()
{
	auto transmit = std::make_unique<Transmit>();

	QString slotHandle;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("SlotHandle"))
		{
			if (mParser->readUniqueElementText(slotHandle))
			{
				transmit->setSlotHandle(slotHandle);
			}
		}
		else if (name == QLatin1String("InputAPDUInfo"))
		{
			parseInputApduInfo(*transmit);
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			mParser->skipCurrentElement();
		}
	}
	if (mParser->parserFailed())
	{
		return nullptr;
	}
	return transmit;
}
