/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ResponseType.h"

#include <QXmlStreamWriter>

namespace governikus
{

class PaosCreator
{
	Q_DISABLE_COPY(PaosCreator)

	public:
		enum class Namespace
		{
			DEFAULT, SOAP, XSD, XSI, PAOS, ADDRESSING, DSS, ECARD, TECHSCHEMA
		};

	private:
		static const QMap<Namespace, QString> mNamespacePrefix;
		static const QMap<Namespace, QString> mNamespace;

		QByteArray mContent;
		QString mRelatedMessageId;
		QXmlStreamWriter mWriter;

		void createEnvelopeElement();
		void createHeaderElement();

	protected:
		void writeTextElement(const QString& pQualifiedName, const QByteArray& pText);
		virtual void createBodyElement(QXmlStreamWriter& pWriter) = 0;

		void createResultElement(const ResponseType& pResponse);

		PaosCreator();
		virtual ~PaosCreator();

	public:
		/**
		 * \brief Creates an XML structure of underlying information.
		 * Be aware that this method will create the structure only once. If you change something
		 * and call marshall() again it won't use the new information.
		 * \return Complete XML structure
		 */
		QByteArray marshall();

		void setRelatedMessageId(const QString& pId);
		[[nodiscard]] static QString getNamespace(Namespace pPrefix);
		[[nodiscard]] static QString getNamespacePrefix(Namespace pPrefix, const QString& pSuffix = QString());
		[[nodiscard]] static QString getNamespaceType(Namespace pPrefix, const QString& pType);
};

} // namespace governikus
