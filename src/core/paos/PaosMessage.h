/*!
 * \brief object represents one paos type
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/PaosType.h"

#include <QXmlStreamAttributes>

class test_PaosMessage;

namespace governikus
{

class PaosMessage
{
	friend class ::test_PaosMessage;

	private:
		QString mMessageID, mRelatesTo;

	protected:
		bool handleWSAddressingHeaders(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes);

	public:
		const PaosType mType;

		PaosMessage(PaosType pType);
		virtual ~PaosMessage();

		const QString& getMessageId() const
		{
			return mMessageID;
		}


		void setMessageId(const QString& messageId)
		{
			mMessageID = messageId;
		}


		const QString& getRelatesTo() const
		{
			return mRelatesTo;
		}


		void setRelatesTo(const QString& relatesTo)
		{
			mRelatesTo = relatesTo;
		}


};

} /* namespace governikus */
