/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "pace/DomainParameterMapping.h"

#include <openssl/ec.h>
#include <QByteArray>
#include <QSharedPointer>

namespace governikus
{

//
// Algorithm according to TR 03110 Part 3 chapter A.3.4.1.Generic Mapping
//
class EcdhGenericMapping
	: public DomainParameterMapping<EC_GROUP>
{
	private:
		QSharedPointer<EC_GROUP> mCurve;
		QSharedPointer<EC_KEY> mTerminalKey;

		QSharedPointer<EC_POINT> createNewGenerator(QSharedPointer<EC_POINT> pCardPubKey, QSharedPointer<BIGNUM> pS);

		void setGenerator(QSharedPointer<EC_POINT> newGenerator);

	public:
		EcdhGenericMapping(QSharedPointer<EC_GROUP> pCurve);
		virtual ~EcdhGenericMapping();

		QByteArray generateTerminalMappingData() override;

		QSharedPointer<EC_GROUP> generateEphemeralDomainParameters(const QByteArray& pCardMappingData, const QByteArray& pNonce) override;
};

} /* namespace governikus */
