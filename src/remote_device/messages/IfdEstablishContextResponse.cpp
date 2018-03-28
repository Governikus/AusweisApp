/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "IfdEstablishContextResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;

namespace
{
VALUE_NAME(IFD_NAME, "IFDName")
}

IfdEstablishContextResponse::IfdEstablishContextResponse(const QString& pIfdName, const QString& pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDEstablishContextResponse, pResultMinor)
	, mIfdName(pIfdName)
{
}


IfdEstablishContextResponse::IfdEstablishContextResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mIfdName()
{
	mIfdName = getStringValue(pMessageObject, IFD_NAME());
}


QJsonDocument IfdEstablishContextResponse::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);
	result[IFD_NAME()] = mIfdName;
	return QJsonDocument(result);
}


const QString& IfdEstablishContextResponse::getIfdName() const
{
	return mIfdName;
}
