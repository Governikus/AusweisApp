/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief  Transmit objects hold a transmit request and provide access to the transmitted data via member functions.
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
		explicit Transmit(const QByteArray& pXmlData);
		~Transmit() override;

		[[nodiscard]] const QString& getSlotHandle() const
		{
			return mSlotHandle;
		}


		void setSlotHandle(const QString& pSlotHandle)
		{
			mSlotHandle = pSlotHandle;
		}


		[[nodiscard]] const QList<InputAPDUInfo>& getInputApduInfos() const
		{
			return mInputApduInfos;
		}


		void appendInputApduInfo(const InputAPDUInfo& pInfo)
		{
			mInputApduInfos += pInfo;
		}


};

} // namespace governikus
