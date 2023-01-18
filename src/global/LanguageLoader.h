/*
 * \brief Loads translation files for different languages.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLocale>
#include <QSharedPointer>
#include <QTranslator>
#include <QVector>

class test_LanguageLoader;
class test_ProviderParser;
class test_ProviderConfigurationParser;

namespace governikus
{

class LanguageLoader
{
	Q_DISABLE_COPY(LanguageLoader)
	friend class ::test_LanguageLoader;
	friend class ::test_ProviderConfigurationParser;

	private:
		static const QLocale::Language mFallbackLanguage;
		static QLocale mDefaultLanguage;

		QString mPath;
		QVector<QSharedPointer<QTranslator>> mTranslatorList;
		const QStringList mComponentList;
		QLocale mUsedLocale;

		bool loadTranslationFiles(const QLocale& pLocale);
		QSharedPointer<QTranslator> createTranslator(const QLocale& pLocale, const QString& pComponent);

	protected:
		LanguageLoader();
		~LanguageLoader() = default;

	public:
		[[nodiscard]] static const QLocale& getDefaultLanguage();
		static void setDefaultLanguage(const QLocale& pLocale);
		static LanguageLoader& getInstance();
		static QString getLocaleCode(const QLocale& pLocale = getInstance().getUsedLocale());

#ifndef QT_NO_DEBUG
		[[nodiscard]] const QString& getPath() const;
		void setPath(const QString& pPath);
#endif

		void load(const QLocale& pLocale = getDefaultLanguage());
		void unload();
		[[nodiscard]] bool isLoaded() const;
		[[nodiscard]] const QLocale& getUsedLocale() const;
		[[nodiscard]] QLocale::Language getFallbackLanguage() const;
		[[nodiscard]] QList<QLocale> getAvailableLocales() const;
};

} // namespace governikus
