/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdMessageResponse.h"

#include <QJsonObject>
#include <QString>


namespace governikus
{
class IfdEstablishContextResponse
	: public IfdMessageResponse
{
	private:
		QString mIfdName;

	public:
		IfdEstablishContextResponse(const QString& pIfdName, ECardApiResult::Minor pResultMinor = ECardApiResult::Minor::null);
		explicit IfdEstablishContextResponse(const QJsonObject& pMessageObject);
		~IfdEstablishContextResponse() override = default;

		[[nodiscard]] const QString& getIfdName() const;
		[[nodiscard]] QByteArray toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const override;
};


} // namespace governikus
