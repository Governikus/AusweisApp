/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>


namespace governikus
{

class PinModify
{
	private:
		quint8 mTimeoutSeconds;

	public:
		explicit PinModify(quint8 pTimeoutSeconds);
		explicit PinModify(const QByteArray& pRemoteInputData);

		[[nodiscard]] quint8 getTimeoutSeconds() const;
		[[nodiscard]] QByteArray createCcid() const;
};

} // namespace governikus
