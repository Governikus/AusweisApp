/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SecureMessagingCommand.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SecureMessagingCommand::SecureMessagingCommand(const CommandApdu& pApdu)
	: SecureMessagingApdu()
	, mExpectedLength()
	, mChecksum()
{
	QByteArray data = pApdu.getData();
	if (!decodeData(data))
	{
		return;
	}

	mExpectedLength = decodeObject<SM_PROTECTED_LE>(data, false);
	if (mExpectedLength)
	{
		data = data.mid(encodeObject(mExpectedLength.data()).length());
	}

	mChecksum = decodeObject<SM_CHECKSUM>(data, false);
	if (mChecksum == nullptr || mChecksum->length != 8)
	{
		qCCritical(card) << "Error on decoding mac";
		return;
	}

	setValid();
}


int SecureMessagingCommand::getExpectedLength() const
{
	const QByteArray expectedLength = Asn1OctetStringUtil::getValue(mExpectedLength.data());

	if (expectedLength.isEmpty())
	{
		return 0;
	}

	if (expectedLength.size() == 1)
	{
		int le = qFromBigEndian<quint8>(expectedLength.data());
		if (le == 0)
		{
			return CommandApdu::SHORT_MAX_LE;
		}
		return le;
	}

	int le = qFromBigEndian<quint16>(expectedLength.data());
	if (le == 0)
	{
		return CommandApdu::EXTENDED_MAX_LE;
	}
	return le;
}


QByteArray SecureMessagingCommand::getExpectedLengthObjectEncoded() const
{
	return encodeObject(mExpectedLength.data());
}


QByteArray SecureMessagingCommand::getMac() const
{
	return Asn1OctetStringUtil::getValue(mChecksum.data());
}
