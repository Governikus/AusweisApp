/*!
 * \brief Base class for PAOS message parsers.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/element/ElementParser.h"
#include "paos/PaosMessage.h"

namespace governikus
{

class PaosParser
	: public ElementParser
{
	public:
		PaosParser(const QString& pMessageName);
		virtual ~PaosParser();

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

}
