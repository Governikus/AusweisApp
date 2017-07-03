/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "pace/DomainParameterMapping.h"

#include <QByteArray>
#include <QSharedPointer>
#include <openssl/ec.h>

namespace governikus
{

//
// Algorithm according to TR 03110 Part 3 chapter A.3.4.1.Generic Mapping
//
class EcdhGenericMapping
	: public DomainParameterMapping<EC_GROUP>
{
	private:
		const QSharedPointer<EC_GROUP> mCurve;
		QSharedPointer<EC_KEY> mTerminalKey;

		QSharedPointer<EC_POINT> createNewGenerator(const QSharedPointer<const EC_POINT>& pCardPubKey, const QSharedPointer<const BIGNUM>& pS);

		void setGenerator(const QSharedPointer<const EC_POINT>& pNewGenerator);

	public:
		EcdhGenericMapping(const QSharedPointer<EC_GROUP>& pCurve);
		virtual ~EcdhGenericMapping();

		QByteArray generateTerminalMappingData() override;

		QSharedPointer<EC_GROUP> generateEphemeralDomainParameters(const QByteArray& pCardMappingData, const QByteArray& pNonce) override;
};

} /* namespace governikus */
