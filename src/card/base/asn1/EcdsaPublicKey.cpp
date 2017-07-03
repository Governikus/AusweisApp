/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "EcdsaPublicKey.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>
#include <openssl/evp.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

int EcdsaPublicKey::decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg)
{
	Q_UNUSED(pIt);
	Q_UNUSED(pExarg);
	if (pOperation == ASN1_OP_D2I_POST)
	{
		if (auto ecdsaPublicKey = reinterpret_cast<EcdsaPublicKey*>(*pVal))
		{
			// According to TR-03110-3, chapter D.3.3:
			// CONDITIONAL domain parameters MUST be either all present, except the cofactor, or all	absent or all absent
			if ((ecdsaPublicKey->mPrimeModulus && ecdsaPublicKey->mFirstCoefficient && ecdsaPublicKey->mSecondCoefficient && ecdsaPublicKey->mBasePoint && ecdsaPublicKey->mOrderOfTheBasePoint)
					|| (!ecdsaPublicKey->mPrimeModulus && !ecdsaPublicKey->mFirstCoefficient && !ecdsaPublicKey->mSecondCoefficient && !ecdsaPublicKey->mBasePoint && !ecdsaPublicKey->mOrderOfTheBasePoint))
			{
				ecdsaPublicKey->initEcKey();
				return CB_SUCCESS;
			}
			else
			{
				qCCritical(card) << "Some conditional domain parameters are present and some are absent";
				EcdsaPublicKey_free(ecdsaPublicKey);
				*pVal = nullptr;
				return CB_ERROR;
			}
		}
	}
	return CB_SUCCESS;
}


// ASN1_SEQUENCE with callback to be able to init EC_KEY after OpenSSL decoding call
ASN1_SEQUENCE_cb(ecdsapublickey_st, EcdsaPublicKey::decodeCallback) = {
	ASN1_SIMPLE(ecdsapublickey_st, mObjectIdentifier, ASN1_OBJECT),
	ASN1_IMP_OPT(ecdsapublickey_st, mPrimeModulus, ASN1_OCTET_STRING, 0x01),
	ASN1_IMP_OPT(ecdsapublickey_st, mFirstCoefficient, ASN1_OCTET_STRING, 0x02),
	ASN1_IMP_OPT(ecdsapublickey_st, mSecondCoefficient, ASN1_OCTET_STRING, 0x03),
	ASN1_IMP_OPT(ecdsapublickey_st, mBasePoint, ASN1_OCTET_STRING, 0x04),
	ASN1_IMP_OPT(ecdsapublickey_st, mOrderOfTheBasePoint, ASN1_OCTET_STRING, 0x05),
	ASN1_IMP(ecdsapublickey_st, mPublicPoint, ASN1_OCTET_STRING, 0x06),
	ASN1_IMP_OPT(ecdsapublickey_st, mCofactor, ASN1_OCTET_STRING, 0x07),
}


ASN1_SEQUENCE_END_cb(ecdsapublickey_st, ecdsapublickey_st)


ASN1_ITEM_TEMPLATE(EcdsaPublicKey) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x49, EcdsaPublicKey, ecdsapublickey_st)
ASN1_ITEM_TEMPLATE_END(EcdsaPublicKey)


IMPLEMENT_ASN1_FUNCTIONS(EcdsaPublicKey)
IMPLEMENT_ASN1_OBJECT(EcdsaPublicKey)


}  // namespace governikus


QSharedPointer<EcdsaPublicKey> EcdsaPublicKey::fromHex(const QByteArray& pHexValue)
{
	return decode(QByteArray::fromHex(pHexValue));
}


QSharedPointer<EcdsaPublicKey> EcdsaPublicKey::decode(const QByteArray& pBytes)
{
	return decodeObject<EcdsaPublicKey>(pBytes);
}


QByteArray EcdsaPublicKey::encode()
{
	return encodeObject(this);
}


QByteArray EcdsaPublicKey::getPublicKeyOid() const
{
	return Asn1ObjectUtil::convertTo(mObjectIdentifier);
}


QByteArray EcdsaPublicKey::getPublicKeyOidValueBytes() const
{
	return QByteArray(reinterpret_cast<const char*>(mObjectIdentifier->data), mObjectIdentifier->length);
}


QByteArray EcdsaPublicKey::getUncompressedPublicPoint() const
{
	return QByteArray(reinterpret_cast<const char*>(mPublicPoint->data), mPublicPoint->length);
}


QSharedPointer<const EC_KEY> EcdsaPublicKey::getEcKey() const
{
	return mEcKey;
}


void EcdsaPublicKey::initEcKey()
{
	if (!mPrimeModulus)
	{
		return;
	}


	QSharedPointer<BIGNUM> p = EcUtil::create(BN_new());
	if (!BN_bin2bn(mPrimeModulus->data, mPrimeModulus->length, p.data()))
	{
		qCCritical(card) << "Cannot convert prime modulus";
		return;
	}

	QSharedPointer<BIGNUM> a = EcUtil::create(BN_new());
	if (!BN_bin2bn(mFirstCoefficient->data, mFirstCoefficient->length, a.data()))
	{
		qCCritical(card) << "Cannot convert first coefficient";
		return;
	}

	QSharedPointer<BIGNUM> b = EcUtil::create(BN_new());
	if (!BN_bin2bn(mSecondCoefficient->data, mSecondCoefficient->length, b.data()))
	{
		qCCritical(card) << "Cannot convert second coefficient";
		return;
	}

	QSharedPointer<EC_GROUP> group = EcUtil::create(EC_GROUP_new_curve_GFp(p.data(), a.data(), b.data(), nullptr));
	if (!group)
	{
		qCCritical(card) << "Cannot create group";
		return;
	}

	QSharedPointer<EC_POINT> generator = EcUtil::create(EC_POINT_new(group.data()));
	if (!EC_POINT_oct2point(group.data(), generator.data(), mBasePoint->data, mBasePoint->length, nullptr))
	{
		qCCritical(card) << "Cannot convert generator";
		return;
	}


	QSharedPointer<BIGNUM> order = EcUtil::create(BN_new());
	if (!BN_bin2bn(mOrderOfTheBasePoint->data, mOrderOfTheBasePoint->length, order.data()))
	{
		qCCritical(card) << "Cannot convert order of the generator";
		return;
	}

	QSharedPointer<BIGNUM> cofactor;
	if (mCofactor)
	{
		cofactor = EcUtil::create(BN_new());
		if (!BN_bin2bn(mCofactor->data, mCofactor->length, cofactor.data()))
		{
			qCCritical(card) << "Cannot convert cofactor";
			return;
		}
	}

	if (!EC_GROUP_set_generator(group.data(), generator.data(), order.data(), cofactor.data()))
	{
		qCCritical(card) << "Cannot set generator";
		return;
	}

	QSharedPointer<EC_KEY> ecKey = EcUtil::create(EC_KEY_new());
	if (!EC_KEY_set_group(ecKey.data(), group.data()))
	{
		qCCritical(card) << "Cannot set group";
		return;
	}

	QSharedPointer<EC_POINT> publicPoint = EcUtil::create(EC_POINT_new(group.data()));
	if (!EC_POINT_oct2point(group.data(), publicPoint.data(), mPublicPoint->data, mPublicPoint->length, nullptr))
	{
		qCCritical(card) << "Cannot convert public point";
		return;
	}
	if (!EC_KEY_set_public_key(ecKey.data(), publicPoint.data()))
	{
		qCCritical(card) << "Cannot set public key";
		return;
	}
	mEcKey = ecKey;
}
