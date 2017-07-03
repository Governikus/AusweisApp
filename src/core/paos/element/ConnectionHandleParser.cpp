/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ConnectionHandleParser.h"

using namespace governikus;

ConnectionHandleParser::ConnectionHandleParser(QSharedPointer<QXmlStreamReader> pXmlReader)
	: ElementParser(pXmlReader)
{

}


ConnectionHandleParser::~ConnectionHandleParser()
{
}


ConnectionHandle ConnectionHandleParser::parse()
{
	ConnectionHandle connectionHandle;

	bool hasNoRecognitionInfoDuplicate = true;
	QString contexthandle, ifdName, slotIndex, cardApplication, slotHandle;
	while (readNextStartElement())
	{
		qCDebug(paos) << mXmlReader->name();
		if (mXmlReader->name() == QLatin1String("ContextHandle"))
		{
			if (readUniqueElementText(contexthandle))
			{
				connectionHandle.setContextHandle(contexthandle);
			}
		}
		else if (mXmlReader->name() == QLatin1String("IFDName"))
		{
			if (readUniqueElementText(ifdName))
			{
				connectionHandle.setIfdName(ifdName);
			}
		}
		else if (mXmlReader->name() == QLatin1String("SlotIndex"))
		{
			if (readUniqueElementText(slotIndex))
			{
				connectionHandle.setSlotIndex(slotIndex);
			}
		}
		else if (mXmlReader->name() == QLatin1String("CardApplication"))
		{
			if (readUniqueElementText(cardApplication))
			{
				connectionHandle.setCardApplication(cardApplication);
			}
		}
		else if (mXmlReader->name() == QLatin1String("SlotHandle"))
		{
			if (readUniqueElementText(slotHandle))
			{
				connectionHandle.setSlotHandle(slotHandle);
			}
		}
		else if (mXmlReader->name() == QLatin1String("RecognitionInfo"))
		{
			// not yet interpreted
			assertNoDuplicateElement(hasNoRecognitionInfoDuplicate);
			mXmlReader->skipCurrentElement();
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << mXmlReader->name();
			mXmlReader->skipCurrentElement();
		}
	}

	return connectionHandle;
}
