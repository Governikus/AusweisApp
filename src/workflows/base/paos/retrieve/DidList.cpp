/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "DidList.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(paos)


using namespace governikus;

DIDList::DIDList(const QByteArray& xmlData)
	: PaosMessage(PaosType::DID_LIST)
	, ElementDetector(xmlData)
{
	parse();
}


void DIDList::parse()
{
	const QStringList expectedElements({
				QStringLiteral("RelatesTo"),
				QStringLiteral("MessageID"),
				QStringLiteral("ContextHandle"),
				QStringLiteral("IFDName"),
				QStringLiteral("SlotIndex"),
				QStringLiteral("CardApplication"),
				QStringLiteral("SlotHandle"),
				QStringLiteral("RecognitionInfo")
			});

	detectStartElements(expectedElements);
}


bool DIDList::handleFoundElementConnectionHandle(const QString& pElementName, const QString& pValue)
{
	if (pElementName == QLatin1String("ContextHandle"))
	{
		mConnectionHandle.setContextHandle(pValue);
	}
	else if (pElementName == QLatin1String("IFDName"))
	{
		mConnectionHandle.setIfdName(pValue);
	}
	else if (pElementName == QLatin1String("SlotIndex"))
	{
		mConnectionHandle.setSlotIndex(pValue);
	}
	else if (pElementName == QLatin1String("CardApplication"))
	{
		mConnectionHandle.setCardApplication(pValue);
	}
	else if (pElementName == QLatin1String("SlotHandle"))
	{
		mConnectionHandle.setSlotHandle(pValue);
	}
	else if (pElementName == QLatin1String("RecognitionInfo"))
	{
		qCWarning(paos) << "Unsupported element 'RecognitionInfo' with value:" << pValue;
	}
	else
	{
		return false;
	}

	return true; // elementName was found
}


bool DIDList::handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes)
{
	if (handleWSAddressingHeaders(pElementName, pValue, pAttributes))
	{
		// handled all WS addressing PAOS header stuff
	}
	else if (handleFoundElementConnectionHandle(pElementName, pValue))
	{
		// handled all ConnectionHandle stuff
	}

	return true;
}


const ConnectionHandle& DIDList::getConnectionHandle() const
{
	return mConnectionHandle;
}
