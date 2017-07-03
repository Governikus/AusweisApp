/*!
 * BluetoothUtils.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BluetoothUtils.h"

using namespace governikus;


ushort BluetoothUtils::getPaddingLength(ushort pParamLen, ushort pPaddingLen)
{
	int needsPaddingLen = pPaddingLen - (pParamLen % pPaddingLen);
	return needsPaddingLen == pPaddingLen ? 0 : needsPaddingLen;
}


void BluetoothUtils::addPadding(QByteArray& pData, const QByteArray& pContent, ushort pPaddingLen)
{
	ushort paddingLength = getPaddingLength(pContent.size(), pPaddingLen);
	for (ushort i = 0; i < paddingLength; ++i)
	{
		pData.append(static_cast<char>(0x00));
	}
}
