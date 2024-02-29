/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Chat.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"

#include <QLoggingCategory>
#include <openssl/asn1.h>
#include <openssl/objects.h>

#include <array>

Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


namespace governikus
{

ASN1_ITEM_TEMPLATE(DISCRETIONARYDATA) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x13, DISCRETIONARYDATA, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(DISCRETIONARYDATA)

// ASN1_SEQUENCE with callback to do more checks on CHAT after OpenSSL decoding call
ASN1_SEQUENCE_cb(chat_st, CHAT::decodeCallback) = {
	ASN1_SIMPLE(chat_st, mType, ASN1_OBJECT),
	ASN1_SIMPLE(chat_st, mTemplate, DISCRETIONARYDATA),
}


ASN1_SEQUENCE_END_cb(chat_st, chat_st)


ASN1_ITEM_TEMPLATE(CHAT) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x4C, CHAT, chat_st)
ASN1_ITEM_TEMPLATE_END(CHAT)


IMPLEMENT_ASN1_FUNCTIONS(CHAT)
IMPLEMENT_ASN1_OBJECT(CHAT)


}  // namespace governikus


int CHAT::decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg)
{
	Q_UNUSED(pIt)
	Q_UNUSED(pExarg)
	if (pOperation == ASN1_OP_D2I_POST)
	{
		if (auto chat = reinterpret_cast<chat_st*>(*pVal))
		{
			if (chat->getTemplate().size() != 5)
			{
				// per definition it's an OCTET STRING of fixed SIZE(5)
				qCDebug(card) << "CHAT Template has wrong size" << chat->getTemplate().size();
				CHAT_free(chat);
				*pVal = nullptr;
				return CB_ERROR;
			}
			else if (chat->getType() != KnownOid::ID_AT)
			{
				// currently we only support Authentication Terminals
				qCDebug(card) << "CHAT type is unsupported" << chat->getType();
				CHAT_free(chat);
				*pVal = nullptr;
				return CB_ERROR;
			}
		}
	}
	return CB_SUCCESS;
}


QSharedPointer<chat_st> CHAT::fromHex(const QByteArray& pHexValue)
{
	return decode(QByteArray::fromHex(pHexValue));
}


QSharedPointer<chat_st> CHAT::decode(const QByteArray& pBytes)
{
	return decodeObject<CHAT>(pBytes);
}


chat_st::chat_st(const chat_st& pCopy)
	: mType(OBJ_dup(pCopy.mType))
	, mTemplate(ASN1_OCTET_STRING_dup(pCopy.mTemplate))
{
}


chat_st::~chat_st()
{
	/*
	 * When creating an instance of CHAT explicitly via the copy constructor, e.g.
	 * new CHAT(otherChatInstance)
	 * we must take care of freeing all properties explicitly also.
	 *
	 * When using the OpenSSL macro system for object creation, the system handles
	 * the memory by invoking malloc and free, so no constructor and no destructor
	 * is called.
	 */
	ASN1_OBJECT_free(mType);
	ASN1_OCTET_STRING_free(mTemplate);
}


Oid CHAT::getType() const
{
	return Oid(mType);
}


QByteArray CHAT::getTemplate() const
{
	return Asn1OctetStringUtil::getValue(mTemplate);
}


AccessRole CHAT::getAccessRole() const
{
	if (mTemplate->length == 0)
	{
		return AccessRole::UNKNOWN;
	}
	return AccessRole(mTemplate->data[0] >> 6);
}


QSet<AccessRight> CHAT::getAccessRights() const
{
	quint64 accessRoleAndRights = 0;
	for (int i = 0; i < mTemplate->length; ++i)
	{
		accessRoleAndRights <<= 8;
		accessRoleAndRights += mTemplate->data[i];
	}

	QSet<AccessRight> accessRights;
	for (auto accessRight : AccessRoleAndRightsUtil::allRights())
	{
		quint64 mask = 1ULL << static_cast<uint>(accessRight);
		if ((accessRoleAndRights & mask) == mask)
		{
			accessRights.insert(accessRight);
		}
	}
	return accessRights;
}


void CHAT::setAccessRights(const QSet<AccessRight>& pAccessRights)
{
	for (auto accessRight : pAccessRights)
	{
		setTemplateBit(static_cast<uint>(accessRight), true);
	}
}


bool CHAT::hasAccessRight(AccessRight pAccessRight) const
{
	return getAccessRights().contains(pAccessRight);
}


void CHAT::removeAllAccessRights()
{
	const auto& rights = getAccessRights();
	for (auto accessRight : rights)
	{
		removeAccessRight(accessRight);
	}
}


void CHAT::removeAccessRight(AccessRight pAccessRight)
{
	setTemplateBit(static_cast<uint>(pAccessRight), false);
}


void chat_st::setTemplateBit(uint pBitIndex, bool pOn)
{
	if (pBitIndex > 39)
	{
		qCCritical(card) << "Setting template bit > 39 not supported";
		return;
	}
	if (mTemplate->length == 0)
	{
		const std::array<uchar, 5> nullBytes {0, 0, 0, 0, 0};
		ASN1_OCTET_STRING_set(mTemplate, nullBytes.data(), static_cast<int>(nullBytes.size()));
	}

	// because pBitIndex < 40, it follows that pBitIndex / 8 <= 4, so byteNumber has no underflow
	auto byteNumber = static_cast<quint8>(4 - (pBitIndex / 8));
	quint8 bitNumberInByte = pBitIndex % 8;
	if (pOn)
	{
		mTemplate->data[byteNumber] = static_cast<uchar>(mTemplate->data[byteNumber] | (0x01 << bitNumberInByte));
	}
	else
	{
		mTemplate->data[byteNumber] = static_cast<uchar>(mTemplate->data[byteNumber] & ~(0x01 << bitNumberInByte));
	}
}
