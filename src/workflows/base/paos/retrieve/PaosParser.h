/*!
 * \brief Base class for PAOS message parsers.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/PaosMessage.h"
#include "paos/element/ElementParser.h"

namespace governikus
{

class PaosParser
	: public ElementParser
{
	public:
		explicit PaosParser(const QString& pMessageName);
		~PaosParser() override;

		PaosMessage* parse(const QByteArray& pXmlData);

	protected:
		virtual PaosMessage* parseMessage() = 0;

	private:
		PaosMessage* parseEnvelope();
		void parseHeader();
		PaosMessage* parseBody();

	protected:
		QString mMessageName;
		QString mMessageID;
		QString mRelatesTo;
};

} // namespace governikus
