/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */


#include "RemoteMessageResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(RESULT_MAJOR, "ResultMajor")
VALUE_NAME(RESULT_MINOR, "ResultMinor")
} // namespace


QJsonObject RemoteMessageResponse::createMessageBody(const QString& pContextHandle) const
{
	QJsonObject result = RemoteMessage::createMessageBody(pContextHandle);
	const ECardApiResult eCardApiResult(mResultMajor, mResultMinor);
	result[RESULT_MAJOR()] = eCardApiResult.getMajorString();
	result[RESULT_MINOR()] = mResultMinor == ECardApiResult::Minor::null ? QJsonValue() : eCardApiResult.getMinorString();
	return result;
}


RemoteMessageResponse::RemoteMessageResponse(RemoteCardMessageType pMessageType, ECardApiResult::Minor pResultMinor)
	: RemoteMessage(pMessageType)
	, mResultMajor(pResultMinor == ECardApiResult::Minor::null ? ECardApiResult::Major::Ok : ECardApiResult::Major::Error)
	, mResultMinor(pResultMinor)
{
}


RemoteMessageResponse::RemoteMessageResponse(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mResultMajor(ECardApiResult::Major::Ok)
	, mResultMinor(ECardApiResult::Minor::null)
{
	const auto& major = getStringValue(pMessageObject, RESULT_MAJOR());
	mResultMajor = ECardApiResult::parseMajor(major);

	if (resultHasError())
	{
		const auto& minor = getStringValue(pMessageObject, RESULT_MINOR());
		mResultMinor = ECardApiResult::parseMinor(minor);
	}
}


bool RemoteMessageResponse::resultHasError() const
{
	return mResultMajor != ECardApiResult::Major::Ok;
}


ECardApiResult::Minor RemoteMessageResponse::getResultMinor() const
{
	return mResultMinor;
}
