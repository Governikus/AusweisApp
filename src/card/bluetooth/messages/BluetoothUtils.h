/*!
 * \brief Some helper utils for bluetooth messages.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>

namespace governikus
{

class BluetoothUtils
{
	private:
		BluetoothUtils() = delete;
		~BluetoothUtils() = delete;
		Q_DISABLE_COPY(BluetoothUtils)

	public:
		static const ushort DEFAULT_PADDING_LENGTH = 4;
		static ushort getPaddingLength(ushort pParamLen, ushort pPaddingLen = DEFAULT_PADDING_LENGTH);
		static void addPadding(QByteArray& pData, const QByteArray& pContent, ushort pPaddingLen = DEFAULT_PADDING_LENGTH);
};

} /* namespace governikus */
