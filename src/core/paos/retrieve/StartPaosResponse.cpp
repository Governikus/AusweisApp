/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StartPaosResponse.h"

using namespace governikus;

StartPaosResponse::StartPaosResponse(const QByteArray& pXmlData)
	: ResponseType(PaosType::STARTPAOS_RESPONSE)
	, ElementDetector(pXmlData)
{
	parse();
	setResult(Result(mResultMajor, mResultMinor, mResultMessage, Origin::Server));
}


void StartPaosResponse::parse()
{
	QStringList expectedElements;
	expectedElements.push_back(QStringLiteral("RelatesTo"));
	expectedElements.push_back(QStringLiteral("MessageID"));

	expectedElements.push_back(QStringLiteral("ResultMajor"));
	expectedElements.push_back(QStringLiteral("ResultMinor"));
	expectedElements.push_back(QStringLiteral("ResultMessage"));

	detectStartElements(expectedElements);
}


bool StartPaosResponse::handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes)
{
	if (handleWSAddressingHeaders(pElementName, pValue, pAttributes))
	{
		// handled all WS addressing PAOS header stuff
	}
	else if (pElementName == QLatin1String("ResultMajor"))
	{
		mResultMajor = pValue;
	}
	else if (pElementName == QLatin1String("ResultMinor"))
	{
		mResultMinor = pValue;
	}
	else if (pElementName == QLatin1String("ResultMessage"))
	{
		mResultMessage = pValue;
	}
	return true;
}
