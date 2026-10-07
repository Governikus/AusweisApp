/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QList>

#include "InputAPDUInfo.h"
#include "paos/PaosMessage.h"

namespace governikus
{
class Transmit
	: public PaosMessage
{
	private:
		QString mSlotHandle;
		QList<InputAPDUInfo> mInputApduInfos;

	public:
		Transmit();
		~Transmit() override;

		[[nodiscard]] const QString& getSlotHandle() const;
		void setSlotHandle(const QString& pSlotHandle);
		[[nodiscard]] const QList<InputAPDUInfo>& getInputApduInfos() const;
		void appendInputApduInfo(const InputAPDUInfo& pInfo);
};

} // namespace governikus
