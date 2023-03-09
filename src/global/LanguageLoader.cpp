/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "LanguageLoader.h"

#include "SingletonHelper.h"

#include <QCoreApplication>
#include <QDir>
#include <QLoggingCategory>
#include <QObject>
#include <QRegularExpression>

Q_DECLARE_LOGGING_CATEGORY(language)

using namespace governikus;

defineSingleton(LanguageLoader)

const QLocale::Language LanguageLoader::mFallbackLanguage = QLocale::Language::English;

QLocale LanguageLoader::mDefaultLanguage = QLocale::system();

LanguageLoader::LanguageLoader()
	: mPath(QStringLiteral(":/translations"))
	, mTranslatorList()
	, mComponentList(
		{
			QStringLiteral("ausweisapp2"), QStringLiteral("qtbase")
		})
	, mUsedLocale(mFallbackLanguage)
{
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [] {
			// Avoid "this" as lambda capture to get more pointer safety.
			if (LanguageLoader::getInstance().isLoaded())
			{
				LanguageLoader::getInstance().unload();
			}
		});
}


const QLocale& LanguageLoader::getDefaultLanguage()
{
	return mDefaultLanguage;
}


void LanguageLoader::setDefaultLanguage(const QLocale& pLocale)
{
	mDefaultLanguage = pLocale;
}


QString LanguageLoader::getLocaleCode(const QLocale& pLocale)
{
	return pLocale.bcp47Name().left(2);
}


#ifndef QT_NO_DEBUG
const QString& LanguageLoader::getPath() const
{
	return mPath;
}


void LanguageLoader::setPath(const QString& pPath)
{
	mPath = pPath;
}


#endif


QSharedPointer<QTranslator> LanguageLoader::createTranslator(const QLocale& pLocale, const QString& pComponent)
{
	auto translator = QSharedPointer<QTranslator>::create();
	translator->setObjectName(pLocale.name() + QLatin1Char('/') + pComponent);
	bool loaded = translator->load(pLocale, pComponent, QStringLiteral("_"), mPath);
	qCDebug(language) << "Load translation:" << pComponent << '|' << loaded;

	if (!loaded)
	{
		translator.clear();
	}

	return translator;
}


QList<QLocale> LanguageLoader::getAvailableLocales() const
{
	qCDebug(language) << "Use path:" << mPath;

	QList<QLocale> list;
	list << mFallbackLanguage;

	QDir path(mPath);
	path.setFilter(QDir::Files);

	const QRegularExpression regex(QStringLiteral("^ausweisapp2_([a-z]{2}(_[A-Z]{2})?)\\.qm$"));
	const auto entryList = path.entryInfoList();
	for (const auto& info : entryList)
	{
		QRegularExpressionMatch match = regex.match(info.fileName());
		if (match.hasMatch() && !match.captured(1).isNull())
		{
			QLocale locale(match.captured(1));
			if (locale != QLocale::C && !list.contains(locale))
			{
				list << locale;
			}
		}
	}

	return list;
}


bool LanguageLoader::isLoaded() const
{
	return !mTranslatorList.isEmpty();
}


void LanguageLoader::load(const QLocale& pLocale)
{
	if (!mTranslatorList.isEmpty())
	{
		qCWarning(language) << "Loader is already in use. You need to unload before you load again...";
		return;
	}

	qCDebug(language) << "Use path:" << mPath;
	bool loaded = false;

	const auto uiLanguages = pLocale.uiLanguages();
	for (const auto& lang : uiLanguages)
	{
		qCDebug(language) << "UI language:" << lang;
		if (!loaded)
		{
			loaded = loadTranslationFiles(QLocale(lang));
		}
	}

	if (!loaded && !pLocale.uiLanguages().contains(pLocale.name().replace(QLatin1Char('_'), QLatin1Char('-'))))
	{
		qCDebug(language) << "No UI translation found... try to use system locale:" << pLocale.name();
		loadTranslationFiles(QLocale(pLocale.name()));
	}
}


void LanguageLoader::unload()
{
	if (mTranslatorList.isEmpty())
	{
		qCWarning(language) << "Cannot unload translation because it is nothing loaded";
	}
	else
	{
		qCDebug(language) << "Unload translations";
		for (const auto& translator : std::as_const(mTranslatorList))
		{
			QCoreApplication::removeTranslator(translator.data());
		}

		mTranslatorList.clear();
		mUsedLocale = mFallbackLanguage;
	}
}


const QLocale& LanguageLoader::getUsedLocale() const
{
	return mUsedLocale;
}


QLocale::Language LanguageLoader::getFallbackLanguage() const
{
	return mFallbackLanguage;
}


bool LanguageLoader::loadTranslationFiles(const QLocale& pLocale)
{
	if (pLocale.language() == mFallbackLanguage || pLocale == QLocale::C)
	{
		qCDebug(language) << "Using fallback language:" << QLocale::languageToString(mFallbackLanguage);
		return true;
	}

	qCDebug(language) << "Try to load translation:" << pLocale;
	bool loaded = false;

	for (const auto& component : mComponentList)
	{
		QSharedPointer<QTranslator> translator = createTranslator(pLocale, component);

		if (!translator.isNull())
		{
			mTranslatorList += translator;
			QCoreApplication::installTranslator(translator.data());
			mUsedLocale = pLocale;
			loaded = true;
		}
	}

	return loaded;
}
