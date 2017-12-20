/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "Disconnect.h"

using namespace governikus;

Disconnect::Disconnect(const QByteArray& xmlData)
	: PaosMessage(PaosType::DISCONNECT)
	, ElementDetector(xmlData)
{
	parse();
}


Disconnect::~Disconnect()
{
}


void Disconnect::parse()
{
	QStringList expectedElements;
	expectedElements.push_back(QStringLiteral("RelatesTo"));
	expectedElements.push_back(QStringLiteral("MessageID"));

	expectedElements.push_back(QStringLiteral("SlotHandle"));
	detectStartElements(expectedElements);
}


bool Disconnect::handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes)
{
	if (handleWSAddressingHeaders(pElementName, pValue, pAttributes))
	{
		// handled all WS addressing PAOS header stuff
	}
	else if (pElementName == QLatin1String("SlotHandle"))
	{
		mSlotHandle = pValue;
	}
	return true;
}


const QString& Disconnect::getSlotHandle() const
{
	return mSlotHandle;
}
