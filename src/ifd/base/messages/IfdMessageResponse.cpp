/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */


#include "IfdMessageResponse.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(RESULT_MAJOR, "ResultMajor")
VALUE_NAME(RESULT_MINOR, "ResultMinor")
} // namespace


QJsonObject IfdMessageResponse::createMessageBody(const QString& pContextHandle) const
{
	QJsonObject result = IfdMessage::createMessageBody(pContextHandle);
	const ECardApiResult eCardApiResult(mResultMajor, mResultMinor);
	result[RESULT_MAJOR()] = eCardApiResult.getMajorString();
	result[RESULT_MINOR()] = mResultMinor == ECardApiResult::Minor::null ? QJsonValue() : eCardApiResult.getMinorString();
	return result;
}


IfdMessageResponse::IfdMessageResponse(IfdMessageType pMessageType, ECardApiResult::Minor pResultMinor)
	: IfdMessage(pMessageType)
	, mResultMajor(pResultMinor == ECardApiResult::Minor::null ? ECardApiResult::Major::Ok : ECardApiResult::Major::Error)
	, mResultMinor(pResultMinor)
{
}


IfdMessageResponse::IfdMessageResponse(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
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


bool IfdMessageResponse::resultHasError() const
{
	return mResultMajor != ECardApiResult::Major::Ok;
}


ECardApiResult::Minor IfdMessageResponse::getResultMinor() const
{
	return mResultMinor;
}
