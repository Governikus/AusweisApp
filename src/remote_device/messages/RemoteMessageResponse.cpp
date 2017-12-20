/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */


#include "RemoteMessageResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


#define RESULTMAJOR "http://www.bsi.bund.de/ecard/api/1.1/resultmajor"


namespace
{
VALUE_NAME(RESULT_MAJOR, "ResultMajor")
VALUE_NAME(RESULT_MINOR, "ResultMinor")
}


QJsonObject RemoteMessageResponse::createMessageBody(const QString& pContextHandle) const
{
	QJsonObject result = RemoteMessage::createMessageBody(pContextHandle);
	result[RESULT_MAJOR()] = mResultMajor;
	result[RESULT_MINOR()] = mResultMinor.isEmpty() ? QJsonValue() : mResultMinor;
	return result;
}


RemoteMessageResponse::RemoteMessageResponse(RemoteCardMessageType pMessageType, const QString& pResultMinor)
	: RemoteMessage(pMessageType)
	, mResultMajor(QStringLiteral(RESULTMAJOR "#ok"))
	, mResultMinor()
{
	if (!pResultMinor.isEmpty())
	{
		mResultMajor = QStringLiteral(RESULTMAJOR "#error");
		mResultMinor = QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor") + pResultMinor;
	}
}


RemoteMessageResponse::RemoteMessageResponse(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mResultMajor()
	, mResultMinor()
{
	mResultMajor = getStringValue(pMessageObject, RESULT_MAJOR());
	if (resultHasError())
	{
		mResultMinor = getStringValue(pMessageObject, RESULT_MINOR());
	}
}


bool RemoteMessageResponse::resultHasError() const
{
	return mResultMajor != QLatin1String(RESULTMAJOR "#ok");
}


const QString& RemoteMessageResponse::getResultMinor() const
{
	return mResultMinor;
}
