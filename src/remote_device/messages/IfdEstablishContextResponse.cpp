/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */


#include "IfdEstablishContextResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;

namespace
{
VALUE_NAME(IFD_NAME, "IFDName")
} // namespace

IfdEstablishContextResponse::IfdEstablishContextResponse(const QString& pIfdName, ECardApiResult::Minor pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDEstablishContextResponse, pResultMinor)
	, mIfdName(pIfdName)
{
}


IfdEstablishContextResponse::IfdEstablishContextResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mIfdName()
{
	mIfdName = getStringValue(pMessageObject, IFD_NAME());

	if (getType() != RemoteCardMessageType::IFDEstablishContextResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDEstablishContextResponse"));
	}
}


QByteArray IfdEstablishContextResponse::toByteArray(const IfdVersion&, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[IFD_NAME()] = mIfdName;

	return RemoteMessage::toByteArray(result);
}


const QString& IfdEstablishContextResponse::getIfdName() const
{
	return mIfdName;
}
