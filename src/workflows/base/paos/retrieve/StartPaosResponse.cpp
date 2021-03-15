/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StartPaosResponse.h"

using namespace governikus;

StartPaosResponse::StartPaosResponse(const QByteArray& pXmlData)
	: ResponseType(PaosType::STARTPAOS_RESPONSE)
	, ElementDetector(pXmlData)
{
	parse();
	setResult(ECardApiResult(mResultMajor, mResultMinor, mResultMessage, ECardApiResult::Origin::Server));
}


void StartPaosResponse::parse()
{
	const QStringList expectedElements({
				QStringLiteral("RelatesTo"),
				QStringLiteral("MessageID"),
				QStringLiteral("ResultMajor"),
				QStringLiteral("ResultMinor"),
				QStringLiteral("ResultMessage")
			});

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
