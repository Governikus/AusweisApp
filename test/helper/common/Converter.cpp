/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "Converter.h"

#include <QList>


using namespace governikus;


QSharedPointer<const CVCertificate> Converter::certificatefromHex(const QByteArray& pBytes)
{
	return CVCertificate::fromRaw(QByteArray::fromHex(pBytes));
}


QByteArray Converter::toHex(const BIGNUM* pBigNum)
{
	if (pBigNum == nullptr)
	{
		qWarning() << "Got nullptr, returning empty QByteArray";
		return QByteArray();
	}

	QList<uchar> buf(BN_num_bytes(pBigNum));
	BN_bn2bin(pBigNum, buf.data());
	return QByteArray(reinterpret_cast<char*>(buf.data()), buf.size()).toHex().toUpper();
}
