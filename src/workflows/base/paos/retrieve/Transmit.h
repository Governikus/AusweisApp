/*!
 * \brief  Transmit objects hold a transmit request and provide access to the transmitted data via member functions.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QVector>

#include "InputAPDUInfo.h"
#include "paos/PaosMessage.h"

namespace governikus
{
class Transmit
	: public PaosMessage
{
	private:
		QString mSlotHandle;
		QVector<InputAPDUInfo> mInputApduInfos;

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


		[[nodiscard]] const QVector<InputAPDUInfo>& getInputApduInfos() const
		{
			return mInputApduInfos;
		}


		void appendInputApduInfo(const InputAPDUInfo& pInfo)
		{
			mInputApduInfos += pInfo;
		}


};

} // namespace governikus
