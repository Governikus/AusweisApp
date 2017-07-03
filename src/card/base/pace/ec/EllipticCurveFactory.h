/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "asn1/SecurityInfo.h"

#include <QByteArray>
#include <QSharedPointer>
#include <openssl/ec.h>

namespace governikus
{


class PACEInfo;


class EllipticCurveFactory
{
	private:
		static QSharedPointer<EC_GROUP> createCurve(int pNid);

	public:
		/*!
		 * \brief Creates an elliptic curve either by using the PACEInfo's parameter id to determine a standardized elliptic curve
		 * or by explicit domain parameters supplied in the corresponding PACEDomainParameterInfo element.
		 * \param pPaceInfo PACEInfo element containing domain parameter
		 * \return elliptic curve object
		 */
		static QSharedPointer<EC_GROUP> create(QSharedPointer<PACEInfo> pPaceInfo);

		/*!
		 * \brief Creates a standardized elliptic curve with specified curve index..
		 * \param pCurveIndex elliptic curve index
		 * \return elliptic curve object
		 */
		static QSharedPointer<EC_GROUP> create(int pCurveIndex);
};

} /* namespace governikus */
