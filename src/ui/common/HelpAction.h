/*!
 * \brief Helper class for mapping object name from f1 widget to help file.
 * \see AppQtMainWidget::onContentActionClicked()
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
	private:
		friend class ::test_HelpAction;

		static const QMap<QString, QString> mHelpMapping;

		Q_DISABLE_COPY(HelpAction)

		QLocale::Language getExistingHelpLanguage() const;
		QString getContextMapping(const QString& pObjectName) const;
		QString getHelpPath(QLocale::Language pLang) const;
		QString getHelpUrl(const QString& pObjectName) const;

	protected:
		static HelpAction& getInstance();
		HelpAction() = default;
		~HelpAction() = default;

	public:
		static QString getOnlineUrl(const QString& pObjectName = QString());
		static void openContextHelp(const QString& pObjectName = QStringLiteral("applicationPage"));
};

} // namespace governikus
