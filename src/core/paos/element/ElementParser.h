/*!
 * \brief Base class for all XML element parser.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QLoggingCategory>
#include <QSharedPointer>
#include <QVector>
#include <QXmlStreamReader>


Q_DECLARE_LOGGING_CATEGORY(paos)


namespace governikus
{

class ElementParser
{
	public:
		ElementParser(QSharedPointer<QXmlStreamReader> pXmlReader);
		virtual ~ElementParser();

	protected:
		// helper methods

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
		 * \return \c true, if the assertion holds, \c false otherwise.
		 */
		bool assertMandatoryElement(const QString& pValue, const char* pElementName);

		/*!
		 * \brief Issues a log warning and sets the error when the list is empty.
		 * \param pList the list to check.
		 * \param pElementName the elements name used to generate the log message.
		 * \return \c true, if the assertion holds, \c false otherwise.
		 */
		template<typename T> bool assertMandatoryList(const QVector<T>& pList, const char* pElementName);

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

		QSharedPointer<QXmlStreamReader> mXmlReader;
		bool mParseError;
};


template<typename T> bool ElementParser::assertMandatoryList(const QVector<T>& pList, const char* pElementName)
{
	if (pList.isEmpty())
	{
		qCWarning(paos) << "Mandatory list is empty:" << pElementName;
		mParseError = true;
		return false;
	}

	return true;
}


} /* namespace governikus */
