/*!
 * \brief Base class for PAOS message parsers.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
