/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "FailureCode.h"


using namespace governikus;


FailureCode::FailureCode(Reason pReason)
	: mReason(pReason)
	, mFailureInfoMap()
{

}


FailureCode::FailureCode(Reason pReason, const FailureInfoMap& pInfoMap)
	: mReason(pReason)
	, mFailureInfoMap(pInfoMap)
{

}


FailureCode::FailureCode(Reason pReason, const QPair<Info, QString>& pInfo)
	: mReason(pReason)
	, mFailureInfoMap({
				{pInfo.first, pInfo.second}
			})
{

}


FailureCode::Reason FailureCode::getReason() const
{
	return mReason;
}


bool FailureCode::operator==(const FailureCode& pFailure) const
{
	return mReason == pFailure.getReason();
}


bool FailureCode::operator!=(const FailureCode& pFailure) const
{
	return mReason != pFailure.getReason();
}


const FailureCode::FailureInfoMap& FailureCode::getFailureInfoMap() const
{
	return mFailureInfoMap;
}


[[nodiscard]] QString FailureCode::toString() const
{
	QString description = QStringLiteral("<b>%1</b>").arg(getEnumName(mReason));

	if (mFailureInfoMap.empty())
	{
		return description;
	}

	description += QStringLiteral("<ul>");
	const auto& keys = mFailureInfoMap.keys();
	for (const auto& key : keys)
	{
		description += QStringLiteral("<li>%1: %2</li>").arg(getEnumName(key), mFailureInfoMap[key]);
	}
	description += QStringLiteral("</ul>");

	return description;
}


QDebug operator <<(QDebug pDbg, const FailureCode& pFailure)
{
	QStringList keyValue;
	const auto& map = pFailure.getFailureInfoMap();
	if (map.isEmpty())
	{
		pDbg << pFailure.getReason();
		return pDbg;
	}

	const auto& keys = map.keys();
	for (const auto key : keys)
	{
		keyValue << QStringLiteral("%1: %2").arg(getEnumName(key), map[key]);
	}
	pDbg << pFailure.getReason() << "[" << keyValue.join(QStringLiteral("; ")) << "]";
	return pDbg;
}
