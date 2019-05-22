/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothUtils.h"

#include <QLoggingCategory>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(bluetooth)


ushort BluetoothUtils::getPaddingLength(ushort pParamLen, ushort pPaddingLen)
{
	ushort needsPaddingLen = static_cast<ushort>(pPaddingLen - (pParamLen % pPaddingLen));
	return needsPaddingLen == pPaddingLen ? 0 : needsPaddingLen;
}


void BluetoothUtils::addPadding(QByteArray& pData, const QByteArray& pContent, ushort pPaddingLen)
{
	if (static_cast<uint>(pContent.size()) > USHRT_MAX)
	{
		qCCritical(bluetooth) << "Content buffer of size bigger than USHRT_MAX not supported";
		Q_ASSERT(static_cast<uint>(pContent.size()) <= USHRT_MAX);
		return;
	}
	ushort paddingLength = getPaddingLength(static_cast<ushort>(pContent.size()), pPaddingLen);
	for (ushort i = 0; i < paddingLength; ++i)
	{
		pData += '\0';
	}
}
