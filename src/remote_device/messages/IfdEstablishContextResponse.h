/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		explicit IfdEstablishContextResponse(const QJsonObject& pMessageObject);
		~IfdEstablishContextResponse() override = default;

		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] QByteArray toByteArray(const IfdVersion& pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
