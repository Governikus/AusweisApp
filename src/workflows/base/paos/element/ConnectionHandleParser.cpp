/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectionHandleParser.h"

#include "ConnectionHandle.h"

using namespace governikus;

ConnectionHandleParser::ConnectionHandleParser(QSharedPointer<QXmlStreamReader> pXmlReader)
	: ElementParser(pXmlReader)
{
}


ConnectionHandleParser::~ConnectionHandleParser() = default;


ConnectionHandle ConnectionHandleParser::parse()
{
	ConnectionHandle connectionHandle;

	QString contexthandle;
	QString ifdName;
	QString slotIndex;
	QString cardApplication;
	QString slotHandle;
	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("ContextHandle"))
		{
			if (readUniqueElementText(contexthandle))
			{
				connectionHandle.setContextHandle(contexthandle);
			}
		}
		else if (name == QLatin1String("IFDName"))
		{
			if (readUniqueElementText(ifdName))
			{
				connectionHandle.setIfdName(ifdName);
			}
		}
		else if (name == QLatin1String("SlotIndex"))
		{
			if (readUniqueElementText(slotIndex))
			{
				connectionHandle.setSlotIndex(slotIndex);
			}
		}
		else if (name == QLatin1String("CardApplication"))
		{
			if (readUniqueElementText(cardApplication))
			{
				connectionHandle.setCardApplication(cardApplication);
			}
		}
		else if (name == QLatin1String("SlotHandle"))
		{
			if (readUniqueElementText(slotHandle))
			{
				connectionHandle.setSlotHandle(slotHandle);
			}
		}
		else if (name == QLatin1String("RecognitionInfo"))
		{
			qCWarning(paos) << "Unsupported element:" << name;
			skipCurrentElement();
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	return parserFailed() ? ConnectionHandle() : connectionHandle;
}
