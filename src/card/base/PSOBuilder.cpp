/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "PSOBuilder.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

/*
 * PSOBuilder
 */
PSOBuilder::PSOBuilder(P1 p1, P2 p2)
	: CommandApduBuilder()
	, mP1(p1)
	, mP2(p2)
	, mCertificateBody()
	, mSignature()
{
}


void PSOBuilder::setCertificateBody(const QByteArray& pData)
{
	mCertificateBody = pData;
}


void PSOBuilder::setSignature(const QByteArray& pData)
{
	mSignature = pData;
}


CommandApdu PSOBuilder::build()
{
	static const int INS = 0x2a;

	QByteArray data;
	data += mCertificateBody;
	data += mSignature;

	return CommandApdu(CommandApdu::CLA, INS, char(mP1), char(mP2), data);
}
