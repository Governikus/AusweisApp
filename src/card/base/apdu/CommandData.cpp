/*!
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "CommandData.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


CommandData::DataEntry::operator QByteArray() const
{
	return Asn1Util::encode(mClass, mTag, mData, mConstructed);
}


CommandData::CommandData(DATA_TAG pTag)
	: mData()
	, mTag(pTag)
{
}


QByteArray CommandData::get(DATA_TAG pTag) const
{
	for (const auto& data : qAsConst(mData))
	{
		if (data.mTag == pTag)
		{
			return data.mData;
		}
	}

	return QByteArray();
}


void CommandData::append(const QByteArray& pData)
{
	const auto* p = reinterpret_cast<const unsigned char*>(pData.data());

	long objectLength = -1;
	int tagNumber = -1;
	int tagClass = -1;

	const int result = ASN1_get_object(&p, &objectLength, &tagNumber, &tagClass, pData.length());
	if (result & 0x80)
	{
		qCDebug(card) << "Parsing failed, an error occurred:" << getOpenSslError();
		return;
	}

	const bool constructed = result & V_ASN1_CONSTRUCTED;
	mData += {tagClass, tagNumber, constructed, QByteArray(reinterpret_cast<const char*>(p), static_cast<int>(objectLength))};
}


void CommandData::append(DATA_TAG pTag, int pValue)
{
	append(pTag, Asn1IntegerUtil::encode(pValue));
}


void CommandData::append(DATA_TAG pTag, const Oid& pOid)
{
	append(pTag, pOid.getData());
}


void CommandData::append(DATA_TAG pTag, PacePasswordId pPassword)
{
	append(pTag, QByteArray(1, Enum<PacePasswordId>::getValue(pPassword)));
}


void CommandData::append(DATA_TAG pTag, const QByteArray& pData)
{
	mData += {V_ASN1_CONTEXT_SPECIFIC, pTag, false, pData};
}


CommandData::operator QByteArray() const
{
	QByteArray data;
	for (const auto& entry : qAsConst(mData))
	{
		data += entry;
	}

	if (mTag == NONE)
	{
		return data;
	}

	return Asn1Util::encode(V_ASN1_APPLICATION, mTag, data, true);
}
