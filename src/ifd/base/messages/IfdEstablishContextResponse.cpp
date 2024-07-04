/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */


#include "IfdEstablishContextResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;

namespace
{
VALUE_NAME(IFD_NAME, "IFDName")
} // namespace

IfdEstablishContextResponse::IfdEstablishContextResponse(const QString& pIfdName, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDEstablishContextResponse, pResultMinor)
	, mIfdName(pIfdName)
{
}


IfdEstablishContextResponse::IfdEstablishContextResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mIfdName()
{
	mIfdName = getStringValue(pMessageObject, IFD_NAME());

	ensureType(IfdMessageType::IFDEstablishContextResponse);
}


QByteArray IfdEstablishContextResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[IFD_NAME()] = mIfdName;

	return IfdMessage::toByteArray(result);
}


const QString& IfdEstablishContextResponse::getIfdName() const
{
	return mIfdName;
}
