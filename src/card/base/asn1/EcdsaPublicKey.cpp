/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "EcdsaPublicKey.h"

#include "ASN1TemplateUtil.h"
#include "ASN1Util.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>

#include <functional>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	#include <openssl/param_build.h>
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace governikus
{

int EcdsaPublicKey::decodeCallback(int pOperation, ASN1_VALUE** pVal, const ASN1_ITEM* pIt, void* pExarg)
{
	Q_UNUSED(pIt)
	Q_UNUSED(pExarg)
	if (pOperation == ASN1_OP_D2I_POST)
	{
		if (auto ecdsaPublicKey = reinterpret_cast<EcdsaPublicKey*>(*pVal))
		{
			// According to TR-03110-3, chapter D.3.3:
			// CONDITIONAL domain parameters MUST be either all present, except the cofactor, or all absent
			if (isAllValid(ecdsaPublicKey) || isAllInvalid(ecdsaPublicKey))
			{
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


bool EcdsaPublicKey::isAllValid(const ecdsapublickey_st* pKey)
{
	return pKey->mPrimeModulus && pKey->mFirstCoefficient && pKey->mSecondCoefficient && pKey->mBasePoint && pKey->mOrderOfTheBasePoint;
}


bool EcdsaPublicKey::isAllInvalid(const ecdsapublickey_st* pKey)
{
	return !pKey->mPrimeModulus && !pKey->mFirstCoefficient && !pKey->mSecondCoefficient && !pKey->mBasePoint && !pKey->mOrderOfTheBasePoint;
}


bool EcdsaPublicKey::isComplete() const
{
	return isAllValid(this);
}


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


SecurityProtocol EcdsaPublicKey::getSecurityProtocol() const
{
	return SecurityProtocol(Oid(mObjectIdentifier));
}


Oid EcdsaPublicKey::getOid() const
{
	return Oid(mObjectIdentifier);
}


QByteArray EcdsaPublicKey::getUncompressedPublicPoint() const
{
	return Asn1OctetStringUtil::getValue(mPublicPoint);
}


[[nodiscard]] EcdsaPublicKey::CurveData EcdsaPublicKey::createCurveData() const
{
	CurveData result;

	result.p = EcUtil::create(BN_new());
	if (!BN_bin2bn(mPrimeModulus->data, mPrimeModulus->length, result.p.data()))
	{
		qCCritical(card) << "Cannot convert prime modulus";
		return {};
	}

	result.a = EcUtil::create(BN_new());
	if (!BN_bin2bn(mFirstCoefficient->data, mFirstCoefficient->length, result.a.data()))
	{
		qCCritical(card) << "Cannot convert first coefficient";
		return {};
	}

	result.b = EcUtil::create(BN_new());
	if (!BN_bin2bn(mSecondCoefficient->data, mSecondCoefficient->length, result.b.data()))
	{
		qCCritical(card) << "Cannot convert second coefficient";
		return {};
	}

	result.order = EcUtil::create(BN_new());
	if (!BN_bin2bn(mOrderOfTheBasePoint->data, mOrderOfTheBasePoint->length, result.order.data()))
	{
		qCCritical(card) << "Cannot convert order of the generator";
		return {};
	}

	if (mCofactor)
	{
		result.cofactor = EcUtil::create(BN_new());
		if (!BN_bin2bn(mCofactor->data, mCofactor->length, result.cofactor.data()))
		{
			qCCritical(card) << "Cannot convert cofactor";
			return {};
		}
	}

	return result;
}


#if OPENSSL_VERSION_NUMBER < 0x30000000L
QSharedPointer<EC_GROUP> EcdsaPublicKey::createGroup(const CurveData& pData) const
{
	QSharedPointer<EC_GROUP> group = EcUtil::create(EC_GROUP_new_curve_GFp(pData.p.data(), pData.a.data(), pData.b.data(), nullptr));
	if (!group)
	{
		qCCritical(card) << "Cannot create group";
		return nullptr;
	}

	QSharedPointer<EC_POINT> generator = EcUtil::create(EC_POINT_new(group.data()));
	if (!EC_POINT_oct2point(group.data(), generator.data(), mBasePoint->data, static_cast<size_t>(mBasePoint->length), nullptr))
	{
		qCCritical(card) << "Cannot convert generator";
		return nullptr;
	}

	if (!EC_GROUP_set_generator(group.data(), generator.data(), pData.order.data(), pData.cofactor.data()))
	{
		qCCritical(card) << "Cannot set generator";
		return nullptr;
	}

	return group;
}


#endif

QSharedPointer<EVP_PKEY> EcdsaPublicKey::createKey(const QByteArray& pPublicPoint) const
{
	return createKey(reinterpret_cast<const uchar*>(pPublicPoint.constData()), pPublicPoint.size());
}


#ifndef QT_NO_DEBUG
QSharedPointer<EVP_PKEY> EcdsaPublicKey::createKey() const
{
	return createKey(reinterpret_cast<const uchar*>(mPublicPoint->data), mPublicPoint->length);
}


#endif

QSharedPointer<EVP_PKEY> EcdsaPublicKey::createKey(const uchar* pPublicPoint, int pPublicPointLength) const
{
	if (!isComplete())
	{
		return nullptr;
	}

	const auto& curveData = createCurveData();
	if (!curveData.isValid())
	{
		return nullptr;
	}

#if OPENSSL_VERSION_NUMBER < 0x30000000L
	const auto& group = createGroup(curveData);
	if (group.isNull())
	{
		return nullptr;
	}

	QSharedPointer<EC_KEY> ecKey = EcUtil::create(EC_KEY_new());
	if (!EC_KEY_set_group(ecKey.data(), group.data()))
	{
		qCCritical(card) << "Cannot set group";
		return nullptr;
	}

	QSharedPointer<EC_POINT> publicPoint = EcUtil::create(EC_POINT_new(group.data()));
	if (!EC_POINT_oct2point(group.data(), publicPoint.data(), pPublicPoint, static_cast<size_t>(pPublicPointLength), nullptr))
	{
		qCCritical(card) << "Cannot convert public point";
		return nullptr;
	}

	if (!EC_KEY_set_public_key(ecKey.data(), publicPoint.data()))
	{
		qCCritical(card) << "Cannot set public key";
		return nullptr;
	}

	const QSharedPointer<EVP_PKEY> key = EcUtil::create(EVP_PKEY_new());
	if (!EVP_PKEY_set1_EC_KEY(key.data(), ecKey.data()))
	{
		qCCritical(card) << "Cannot set key";
		return nullptr;
	}

	return key;

#else
	const auto& params = EcUtil::create([&curveData, pPublicPoint, pPublicPointLength, this](OSSL_PARAM_BLD* pBuilder){
			return OSSL_PARAM_BLD_push_BN(pBuilder, "p", curveData.p.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "a", curveData.a.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "b", curveData.b.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "order", curveData.order.data())
				   && OSSL_PARAM_BLD_push_BN(pBuilder, "cofactor", curveData.cofactor.data())
				   && OSSL_PARAM_BLD_push_octet_string(pBuilder, "pub", pPublicPoint, static_cast<size_t>(pPublicPointLength))
				   && OSSL_PARAM_BLD_push_octet_string(pBuilder, "generator", mBasePoint->data, static_cast<size_t>(mBasePoint->length))
				   && OSSL_PARAM_BLD_push_utf8_string(pBuilder, "field-type", "prime-field", 12);
		});

	if (params == nullptr)
	{
		qCCritical(card) << "Cannot set parameter";
		return nullptr;
	}

	auto ctx = EcUtil::create(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
	if (!EVP_PKEY_fromdata_init(ctx.data()))
	{
		qCCritical(card) << "Cannot init pkey";
		return nullptr;
	}

	EVP_PKEY* key = nullptr;
	if (!EVP_PKEY_fromdata(ctx.data(), &key, EVP_PKEY_PUBLIC_KEY, params.data()))
	{
		qCCritical(card) << "Cannot fetch data for pkey";
		return nullptr;
	}

	return EcUtil::create(key);

#endif
}
