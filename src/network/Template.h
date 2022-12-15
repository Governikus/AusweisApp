/*!
 * A simple template renderer.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QFile>
#include <QMap>
#include <QSet>
#include <QString>


namespace governikus
{

class Template
{
	private:
		QString mTemplate;
		QSet<QString> mKeys;
		QMap<QString, QString> mContext;

	public:
		/*!
		 * \brief Construct a template from file
		 */
		static Template fromFile(const QString& pTemplateFileName);

		/*!
		 * \brief Construct the template \a pTemplate
		 */
		explicit Template(const QString& pTemplate);

		/*!
		 * \brief Get template context keys, i.e. the possible keys
		 * to add as context parameter.
		 */
		[[nodiscard]] const QSet<QString>& getContextKeys() const;

		/*!
		 * \brief Set known context key as parameter. Unknown context
		 * keys are ignored, already set context values are overwritten.
		 *
		 * \return true, if the context key is known, false otherwise
		 */
		bool setContextParameter(const QString& pKey, const QString& pValue);

		/*!
		 * \brief Renders the template by replacing all contained context
		 * keys by context values. If for a contained key no value is specified,
		 * the key is replaced by the empty string.
		 */
		[[nodiscard]] QString render() const;
};

} // namespace governikus
