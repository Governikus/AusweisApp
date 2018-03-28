/*!
 * \brief Parser to decapsulation EstablishPACEChannel
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EstablishPACEChannel.h"


namespace governikus
{

class EstablishPACEChannelParser
{
	private:
		PACE_PASSWORD_ID mPasswordId;
		QByteArray mChat;
		QByteArray mCertificateDescription;
		QByteArray mCommandData;

		EstablishPACEChannelParser(PACE_PASSWORD_ID pPasswordId = PACE_PASSWORD_ID::PACE_PIN,
				const QByteArray& pChat = QByteArray(),
				const QByteArray& pCertificateDescription = QByteArray(),
				const QByteArray& pCommandData = QByteArray());

	public:
		static EstablishPACEChannelParser fromCcid(const QByteArray& pInput);

		PACE_PASSWORD_ID getPasswordId() const;
		const QByteArray& getChat() const;
		const QByteArray& getCertificateDescription() const;
		const QByteArray& getCommandData() const;

};

}
