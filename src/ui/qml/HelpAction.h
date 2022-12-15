/*!
 * \brief Helper class for mapping object name to help file.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLocale>
#include <QMap>
#include <QString>

class test_HelpAction;

namespace governikus
{

class HelpAction
{
	Q_DISABLE_COPY(HelpAction)
	friend class ::test_HelpAction;

	private:
		static const QMap<QString, QString> mQmlHelpMapping;
		static const QString mBaseUrl;

		[[nodiscard]] QLocale::Language getExistingHelpLanguage() const;
		[[nodiscard]] QString getContextMapping(const QString& pObjectName) const;
		[[nodiscard]] QString getHelpPath(QLocale::Language pLang) const;
		[[nodiscard]] QString getHelpUrl(const QString& pObjectName) const;

	protected:
		static HelpAction& getInstance();
		HelpAction() = default;
		~HelpAction() = default;

	public:
		[[nodiscard]] static QString getOnlineUrl(const QString& pObjectName = QString());
		static void openContextHelp(const QString& pObjectName = QStringLiteral("applicationPage"));
};

} // namespace governikus
