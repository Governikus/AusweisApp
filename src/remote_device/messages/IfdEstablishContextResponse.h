/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteMessageResponse.h"

#include <QJsonObject>
#include <QString>


namespace governikus
{
class IfdEstablishContextResponse
	: public RemoteMessageResponse
{
	private:
		QString mIfdName;

	public:
		IfdEstablishContextResponse(const QString& pIfdName, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		IfdEstablishContextResponse(const QJsonObject& pMessageObject);
		virtual ~IfdEstablishContextResponse() override = default;

		const QString& getIfdName() const;
		virtual QByteArray toByteArray(const QString& pContextHandle) const override;
};


} // namespace governikus
