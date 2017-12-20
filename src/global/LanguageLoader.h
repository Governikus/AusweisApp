/*
 * \brief Loads translation files for different languages.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLocale>
#include <QPointer>
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
	private:
		friend class ::test_LanguageLoader;
		friend class ::test_ProviderParser;
		friend class ::test_ProviderConfigurationParser;
		static const QLocale::Language mFallbackLanguage;
		static QLocale mDefaultLanguage;

		QString mPath;
		QVector<QSharedPointer<QTranslator> > mTranslatorList;
		const QStringList mComponentList;
		QLocale mUsedLocale;

		Q_DISABLE_COPY(LanguageLoader)

		bool loadTranslationFiles(const QLocale& pLocale);
		QSharedPointer<QTranslator> createTranslator(const QLocale& pLocale, const QString& pComponent);

	protected:
		LanguageLoader();
		~LanguageLoader();

	public:
		static const QLocale& getDefaultLanguage();
		static void setDefaultLanguage(const QLocale& pLocale);
		static LanguageLoader& getInstance();

#ifndef QT_NO_DEBUG
		const QString& getPath();
		void setPath(const QString& pPath);
#endif

		void load(const QLocale& pLocale = getDefaultLanguage());
		void unload();
		bool isLoaded() const;
		const QLocale& getUsedLocale() const;
		QLocale::Language getFallbackLanguage() const;
		QList<QLocale> getAvailableLocales() const;
};

} /* namespace governikus */
