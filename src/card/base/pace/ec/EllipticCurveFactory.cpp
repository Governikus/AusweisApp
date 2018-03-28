/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include <openssl/evp.h>
#include <openssl/obj_mac.h>

#include "asn1/PACEInfo.h"
#include "pace/ec/EcUtil.h"
#include "pace/ec/EllipticCurveFactory.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


QSharedPointer<EC_GROUP> EllipticCurveFactory::create(const QSharedPointer<const PACEInfo>& pPaceInfo)
{
	if (pPaceInfo->isStandardizedDomainParameters())
	{
		return create(pPaceInfo->getParameterIdAsInt());
	}
	else
	{
		/*
		 * To support creation of elliptic curves with explicit domain parameters,
		 * parse the PACEDomainParameterInfo object from EF.CardAccess and use the
		 * parameters to create the curve.
		 */
		qCCritical(card) << "Creation of elliptic curves by explicit domain parameters not supported";
		return QSharedPointer<EC_GROUP>();
	}
}


QSharedPointer<EC_GROUP> EllipticCurveFactory::createCurve(int pNid)
{
	qCDebug(card) << "Create elliptic curve " << OBJ_nid2sn(pNid);
	EC_GROUP* ecGroup = EC_GROUP_new_by_curve_name(pNid);
	if (ecGroup == nullptr)
	{
		qCCritical(card) << "Error on EC_GROUP_new_by_curve_name, curve is unknown:" << pNid;
	}
	return EcUtil::create(ecGroup);
}


QSharedPointer<EC_GROUP> EllipticCurveFactory::create(int pCurveIndex)
{
	switch (pCurveIndex)
	{
		case 8:
			// According to RFC 5480 2.1.1.1 secp192r1 equals NID_X9_62_prime192v1
			return createCurve(NID_X9_62_prime192v1);

		case 9:
			return createCurve(NID_brainpoolP192r1);

		case 10:
			// cannot be used with IM
			return createCurve(NID_secp224r1);

		case 11:
			return createCurve(NID_brainpoolP224r1);

		case 12:
			// According to RFC 5480 2.1.1.1 secp256r1 equals NID_X9_62_prime256v1
			return createCurve(NID_X9_62_prime256v1);

		case 13:
			return createCurve(NID_brainpoolP256r1);

		case 14:
			return createCurve(NID_brainpoolP320r1);

		case 15:
			return createCurve(NID_secp384r1);

		case 16:
			return createCurve(NID_brainpoolP384r1);

		case 17:
			return createCurve(NID_brainpoolP512r1);

		case 18:
			return createCurve(NID_secp521r1);

		default:
			qCWarning(card) << "Creation of standardized elliptic curve " << pCurveIndex << " not supported";
			return EcUtil::create(static_cast<EC_GROUP*>(nullptr));
	}
}
