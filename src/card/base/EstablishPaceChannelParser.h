/*!
 * \brief Parser to decapsulation EstablishPACEChannel
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EstablishPaceChannel.h"


namespace governikus
{

class EstablishPaceChannelParser
{
	private:
		PacePasswordId mPasswordId;
		QByteArray mChat;
		QByteArray mCertificateDescription;
		QByteArray mCommandData;

		EstablishPaceChannelParser(PacePasswordId pPasswordId = PacePasswordId::PACE_PIN,
				const QByteArray& pChat = QByteArray(),
				const QByteArray& pCertificateDescription = QByteArray(),
				const QByteArray& pCommandData = QByteArray());

	public:
		static EstablishPaceChannelParser fromCcid(const QByteArray& pInput);

		PacePasswordId getPasswordId() const;
		const QByteArray& getChat() const;
		const QByteArray& getCertificateDescription() const;
		const QByteArray& getCommandData() const;

};

} // namespace governikus
