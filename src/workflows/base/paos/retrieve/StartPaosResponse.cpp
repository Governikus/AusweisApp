/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StartPaosResponse.h"

using namespace governikus;

StartPaosResponse::StartPaosResponse(const QByteArray& pXmlData)
	: ResponseType(PaosType::STARTPAOS_RESPONSE)
	, ElementDetector(pXmlData)
	, mResultMajor()
	, mResultMinor()
	, mResultMessage()
	, mStatusCode(0)
	, mRemainingDays(-1)
	, mRemainingAttempts(-1)
	, mBlockingCode()
{
	parse();
	setResult(ECardApiResult(mResultMajor, mResultMinor, mResultMessage, ECardApiResult::Origin::Server));
}


int StartPaosResponse::getStatusCode() const
{
	return mStatusCode;
}


int StartPaosResponse::getRemainingDays() const
{
	return mRemainingDays;
}


int StartPaosResponse::getRemainingAttempts() const
{
	return mRemainingAttempts;
}


const QString& StartPaosResponse::getBlockingCode() const
{
	return mBlockingCode;
}


void StartPaosResponse::parse()
{
	const QStringList expectedElements({
				QStringLiteral("RelatesTo"),
				QStringLiteral("MessageID"),
				QStringLiteral("ResultMajor"),
				QStringLiteral("ResultMinor"),
				QStringLiteral("ResultMessage"),
				QStringLiteral("statusCode"),
				QStringLiteral("remainingDays"),
				QStringLiteral("remainingAttempts"),
				QStringLiteral("blockingCode")
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
	else if (pElementName == QLatin1String("statusCode"))
	{
		mStatusCode = valuetoInt(pValue);
	}
	else if (pElementName == QLatin1String("remainingDays"))
	{
		mRemainingDays = valuetoInt(pValue);
	}
	else if (pElementName == QLatin1String("remainingAttempts"))
	{
		mRemainingAttempts = valuetoInt(pValue);
	}
	else if (pElementName == QLatin1String("blockingCode"))
	{
		mBlockingCode = pValue;
	}

	return true;
}


int StartPaosResponse::valuetoInt(const QString& pValue) const
{
	bool succeed = false;
	const int value = pValue.toInt(&succeed);
	return succeed ? value : -1;
}
