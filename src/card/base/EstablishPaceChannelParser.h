/*!
 * \brief Parser to decapsulation EstablishPACEChannel
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
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

	public:
		EstablishPaceChannelParser();

		PacePasswordId getPasswordId() const;
		const QByteArray& getChat() const;
		const QByteArray& getCertificateDescription() const;
		const QByteArray& getCommandData() const;

		void fromCcid(const QByteArray& pInput);
		void reset();
};

} // namespace governikus
