/*!
 * \brief Class represents the retrieved PAOS Disconnect.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ElementDetector.h"
#include "paos/PaosMessage.h"

namespace governikus
{

class Disconnect
	: public PaosMessage
	, private ElementDetector
{
	private:
		QString mSlotHandle;

		void parse();
		bool handleFoundElementConnectionHandle(const QString& pElementName, const QString& pValue);
		virtual bool handleFoundElement(const QString& pElementName, const QString& pValue, const QXmlStreamAttributes& pAttributes) override;

	public:
		Disconnect(const QByteArray& pXmlData);
		virtual ~Disconnect() override;

		const QString& getSlotHandle() const;
};

} /* namespace governikus */
