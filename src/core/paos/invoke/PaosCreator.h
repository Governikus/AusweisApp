/*!
 * \brief Base class to create a PaosMessage.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "Result.h"

#include <QDomDocument>

class test_PaosCreator;

namespace governikus
{

class PaosCreator
{
	friend class ::test_PaosCreator;

	public:
		enum class Namespace
		{
			DEFAULT, SOAP, XSD, XSI, PAOS, ADDRESSING, DSS, ECARD, TECHSCHEMA
		};

	private:
		static const QMap<Namespace, QString> mNamespacePrefix;
		static const QMap<Namespace, QString> mNamespace;

		Q_DISABLE_COPY(PaosCreator)

	protected:
		QDomDocument mDoc;

		virtual QDomElement getDocumentStructure() = 0;
		QDomElement createTextElement(const QString& pName, const QByteArray& pContent);
		QDomElement createTextElement(const QString& pName, const QString& pContent);
		QDomElement createTextElement(Namespace pNamespace, const QString& pName, const QByteArray& pContent);
		QDomElement createTextElement(Namespace pNamespace, const QString& pName, const QString& pContent);
		QDomElement createBodyElement(const QDomElement& pBody);
		QDomElement createHeaderElement(const QString& pRrelatesTo, const QString& pMessageID);
		QDomElement createEnvelopeElement(const QDomElement& pBody, const QString& pRelatesTo, const QString& pMessageID);

		QDomElement createResultElement();

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

		static QString getNamespace(Namespace pPrefix);
		static QString getNamespacePrefix(Namespace pPrefix, const QString& pSuffix = QString());
		static QString getNamespaceType(Namespace pPrefix, const QString& pType);
};

} /* namespace governikus */
