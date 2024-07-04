/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectionHandleParser.h"

#include "ConnectionHandle.h"

#include <QLoggingCategory>


using namespace governikus;
using namespace std::placeholders;


Q_DECLARE_LOGGING_CATEGORY(paos)


ConnectionHandleParser::ConnectionHandleParser(QSharedPointer<QXmlStreamReader> pXmlReader)
	: ElementParser(pXmlReader)
{
}


ConnectionHandleParser::~ConnectionHandleParser() = default;


void ConnectionHandleParser::parseUniqueElementText(const std::function<void(const QString&)>& pFunc, QString& pText)
{
	if (readUniqueElementText(pText))
	{
		pFunc(pText);
	}
}


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
			parseUniqueElementText(std::bind(&ConnectionHandle::setContextHandle, &connectionHandle, _1), contexthandle);
			continue;
		}
		if (name == QLatin1String("IFDName"))
		{
			parseUniqueElementText(std::bind(&ConnectionHandle::setIfdName, &connectionHandle, _1), ifdName);
			continue;
		}
		if (name == QLatin1String("SlotIndex"))
		{
			parseUniqueElementText(std::bind(&ConnectionHandle::setSlotIndex, &connectionHandle, _1), slotIndex);
			continue;
		}
		if (name == QLatin1String("CardApplication"))
		{
			parseUniqueElementText(std::bind(&ConnectionHandle::setCardApplication, &connectionHandle, _1), cardApplication);
			continue;
		}
		if (name == QLatin1String("SlotHandle"))
		{
			parseUniqueElementText(std::bind(&ConnectionHandle::setSlotHandle, &connectionHandle, _1), slotHandle);
			continue;
		}
		if (name == QLatin1String("RecognitionInfo"))
		{
			qCWarning(paos) << "Unsupported element:" << name;
			skipCurrentElement();
			continue;
		}

		qCWarning(paos) << "Unknown element:" << name;
		skipCurrentElement();
	}

	return parserFailed() ? ConnectionHandle() : connectionHandle;
}
