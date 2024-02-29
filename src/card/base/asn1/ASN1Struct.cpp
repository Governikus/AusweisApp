/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ASN1Struct.h"

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/ASN1Util.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


ASN1Struct::ASN1Struct(int pClass, DATA_TAG pTag, const QByteArray& pData)
	: mClass(pClass)
	, mTag(pTag)
	, mSimpleData(pData)
	, mComplexData()
{
}


ASN1Struct::ASN1Struct(const QByteArray& pData)
	: mClass(V_ASN1_UNIVERSAL)
	, mTag(NONE)
	, mSimpleData()
	, mComplexData()
{
	QByteArray data(pData);
	while (!data.isEmpty())
	{
		const unsigned char* p = reinterpret_cast<unsigned char*>(data.data());
		long size = -1;
		int tagNumber = -1;
		int tagClass = -1;
		const auto result = ASN1_get_object(&p, &size, &tagNumber, &tagClass, static_cast<long>(data.length()));
		if (result & 0x80)
		{
			qCritical() << "Could not parse ASN1Struct:" << getOpenSslError();
			return;
		}

		const auto dataSize = static_cast<int>(size);
		const auto objectSize = ASN1_object_size(false, dataSize, tagNumber);
		if (data.size() < objectSize)
		{
			qCritical() << "Unexpected end of ASN1Struct";
			return;
		}

		if (mTag != NONE || data.size() > objectSize || !mComplexData.isEmpty())
		{
			mComplexData += ASN1Struct(data.mid(0, objectSize));
			data.remove(0, objectSize);
			continue;
		}

		mClass = tagClass;
		mTag = tagNumber;
		if (result & V_ASN1_CONSTRUCTED)
		{
			data.remove(0, objectSize - dataSize);
			continue;
		}

		mSimpleData.append(reinterpret_cast<const char*>(p), dataSize);
		return;
	}
}


QByteArray ASN1Struct::getData(int pClass, DATA_TAG pTag) const
{
	if (mClass == pClass && mTag == pTag && !mSimpleData.isNull())
	{
		return mSimpleData;
	}

	for (const auto& data : std::as_const(mComplexData))
	{
		const auto& result = data.getData(pClass, pTag);
		if (!result.isNull())
		{
			return result;
		}
	}

	return QByteArray();
}


QByteArray ASN1Struct::getObject(int pClass, DATA_TAG pTag) const
{
	if (mClass == pClass && mTag == pTag)
	{
		return QByteArray(*this);
	}

	for (const auto& data : std::as_const(mComplexData))
	{
		const auto& result = data.getObject(pClass, pTag);
		if (!result.isNull())
		{
			return result;
		}
	}

	return QByteArray();
}


void ASN1Struct::append(const QByteArray& pData)
{
	mComplexData += ASN1Struct(pData);
}


void ASN1Struct::append(DATA_TAG pTag, int pValue)
{
	QSharedPointer<ASN1_TYPE> type(ASN1_TYPE_new(), [](ASN1_TYPE* pType){ASN1_TYPE_free(pType);});
	type->type = V_ASN1_INTEGER;
	type->value.integer = ASN1_INTEGER_new();
	if (!ASN1_INTEGER_set_int64(type->value.integer, pValue))
	{
		qCCritical(card) << "Encoding error on ASN1_INTEGER";
	}
	ASN1Struct integer(Asn1TypeUtil::encode(type.data()));
	append(pTag, integer.getData(V_ASN1_UNIVERSAL, UNI_INTEGER));
}


void ASN1Struct::append(DATA_TAG pTag, const Oid& pOid)
{
	append(pTag, pOid.getData());
}


void ASN1Struct::append(DATA_TAG pTag, PacePasswordId pPassword)
{
	append(pTag, QByteArray(1, Enum<PacePasswordId>::getValue(pPassword)));
}


void ASN1Struct::append(DATA_TAG pTag, const QByteArray& pData)
{
	mComplexData += ASN1Struct(V_ASN1_CONTEXT_SPECIFIC, pTag, pData);
}


ASN1Struct::operator QByteArray() const
{
	if (mSimpleData.isNull())
	{
		QByteArray data;
		for (const auto& entry : std::as_const(mComplexData))
		{
			data += entry;
		}

		if (mTag == NONE)
		{
			return data;
		}

		return Asn1Util::encode(mClass, mTag, data, true);
	}

	return Asn1Util::encode(mClass, mTag, mSimpleData);
}


#ifndef QT_NO_DEBUG
int ASN1Struct::getObjectCount() const
{
	int counter = 1;
	for (const auto& entry : std::as_const(mComplexData))
	{
		counter += entry.getObjectCount();
	}
	return counter;
}


#endif
