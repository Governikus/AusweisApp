/*!
 * \brief Base class to create a PaosMessage.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ResponseType.h"

#include <QXmlStreamWriter>

namespace governikus
{

class PaosCreator
{
	public:
		enum class Namespace
		{
			DEFAULT, SOAP, XSD, XSI, PAOS, ADDRESSING, DSS, ECARD, TECHSCHEMA
		};

	private:
		static const QMap<Namespace, QString> mNamespacePrefix;
		static const QMap<Namespace, QString> mNamespace;

		Q_DISABLE_COPY(PaosCreator)

		QByteArray mContent;
		QString mRelatedMessageId;

		void createEnvelopeElement();
		void createHeaderElement();

	protected:
		QXmlStreamWriter mWriter;

		void writeTextElement(const QString& pQualifiedName, const QByteArray& pText);
		virtual void createBodyElement() = 0;

		void createResultElement(const ResponseType& pResponse);

		PaosCreator();
		virtual ~PaosCreator();

	public:
		/**
		 * \brief Creates an XML structure of underyling information.
		 * Be aware that this method will create the structure only once. If you change something
		 * and call marshall() again it won't use the new information.
		 * \return Complete XML structure
		 */
		QByteArray marshall();

		void setRelatedMessageId(const QString& pId);
		static QString getNamespace(Namespace pPrefix);
		static QString getNamespacePrefix(Namespace pPrefix, const QString& pSuffix = QString());
		static QString getNamespaceType(Namespace pPrefix, const QString& pType);
};

} // namespace governikus
