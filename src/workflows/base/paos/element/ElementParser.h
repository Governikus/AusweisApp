/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/element/ConnectionHandle.h"

#include <QList>
#include <QLoggingCategory>
#include <QSharedPointer>
#include <QXmlStreamReader>


Q_DECLARE_LOGGING_CATEGORY(paos)

class test_ElementParser;

namespace governikus
{

class ElementParser
{
	friend class ::test_ElementParser;

	public:
		explicit ElementParser(QSharedPointer<QXmlStreamReader> pXmlReader);
		virtual ~ElementParser();

		[[nodiscard]] bool parserFailed() const;

	protected:
		// helper methods
		[[nodiscard]] ConnectionHandle parseConnectionHandle();

		/*!
		 * \brief Like QXmlStreamReader::readNextStartElement(), but also checks mParseError.
		 * \return \c true, if a next start element was found, \c false, if the end element was reached or an error occurred.
		 */
		bool readNextStartElement();

		/*!
		 * \brief Returns the text (simplified()) between the current start element and the corresponding end element.
		 * \return The element text on success (may be isEmpty(), but not isNull()), QString() on error.
		 */
		QString readElementText();

		/*!
		 * \brief Issues a log warning and sets the error when the element has not been set, i.e. the element is null.
		 * \param pValue the elements value to check.
		 * \param pElementName the elements name used to generate the log message.
		 */
		void assertMandatoryElement(const QString& pValue, const char* const pElementName);

		/*!
		 * \brief Issues a log warning and sets the error when the list is empty.
		 * \param pList the list to check.
		 * \param pElementName the elements name used to generate the log message.
		 * \return \c true, if the assertion holds, \c false otherwise.
		 */
		template<typename T> bool assertMandatoryList(const QList<T>& pList, const char* const pElementName);

		/*!
		 * \brief Issues a log warning and sets the error when a duplicate element has been encountered.
		 * \param pNotYetSeen Indicates whether the element has not yet been seen.
		 * \return \c true, if the assertion holds, \c false otherwise.
		 */
		bool assertNoDuplicateElement(bool pNotYetSeen);

		/*!
		 * \brief Returns the text (simplified()) between the current start element and
		 * the corresponding end element, if the element has not been encountered yet.
		 *
		 * \param pText The variable that is set to the read text. It is also used as an
		 * indicator whether the element has not yet been encountered (isNull()).
		 *
		 * \return \c true, if the element has not yet been encountered and the element
		 * text could be read, \c false otherwise.
		 */
		bool readUniqueElementText(QString& pText);

		/*!
		 * \brief Skips parsing of the current element
		 * Used to ignore unknown or unrecognized elements while parsing.
		 */
		void skipCurrentElement() const;

		/*!
		 * \brief Returns the name of the current element and issues a log message with it
		 * \return the name of the currently parsed element, e.g. "data" in "<data>...</data>"
		 */
		[[nodiscard]] QStringView getElementName() const;

		void setParserFailed();

		void initData(const QByteArray& pXmlData);

		[[nodiscard]] QStringView getElementTypeByNamespace(const QString& pNamespace) const;

	private:
		QSharedPointer<QXmlStreamReader> mXmlReader;
		bool mParseError;
};


template<typename T> bool ElementParser::assertMandatoryList(const QList<T>& pList, const char* const pElementName)
{
	if (pList.isEmpty())
	{
		qCWarning(paos) << "Mandatory list is empty:" << pElementName;
		mParseError = true;
		return false;
	}

	return true;
}


} // namespace governikus
