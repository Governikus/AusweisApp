/*!
 * UpdateSettingsBackend.h
 *
 * \brief Template class that parses and updates settings.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "FileDestination.h"

#include <QByteArray>
#include <QFile>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(update)

namespace governikus
{

template<typename SettingsClass>
class UpdateSettingsBackend
	: public UpdateBackend
{
	private:
		SettingsClass& mCurrentSettings;

		const QSharedPointer<SettingsParser<SettingsClass> > mParser;

		const QString mDefaultSettingsFilename;

		const QString mNameForLog;

		QSharedPointer<SettingsClass> loadDefaultSettings()
		{
			QFile file(FileDestination::getPath(mDefaultSettingsFilename));
			if (!file.exists())
			{
				qCCritical(update) << "Cannot find file" << mDefaultSettingsFilename;

				return QSharedPointer<SettingsClass>();
			}

			if (!file.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Unbuffered))
			{
				qCCritical(update) << "Cannot open file" << mDefaultSettingsFilename;

				return QSharedPointer<SettingsClass>();
			}

			const QByteArray data = file.readAll();
			file.close();
			QSharedPointer<SettingsClass> defaultSettings = mParser->parse(data);
			if (defaultSettings.isNull())
			{
				qCCritical(update) << QStringLiteral("Parsing of default %1 failed").arg(mNameForLog);

				return QSharedPointer<SettingsClass>();
			}

			qCDebug(update) << QStringLiteral("Loaded default %1 issued on %2").arg(mNameForLog, defaultSettings->getIssueDate().toString(Qt::DateFormat::ISODate));

			return defaultSettings;
		}


		void saveLatestSettings(QSharedPointer<SettingsClass> pUpdatedSettings)
		{
			if (pUpdatedSettings != nullptr && pUpdatedSettings->getIssueDate() > mCurrentSettings.getIssueDate())
			{
				qCDebug(update) << QStringLiteral("Replacing current %1 with updated %1 issued on %2").arg(mNameForLog, pUpdatedSettings->getIssueDate().toString(Qt::DateFormat::ISODate));
				mCurrentSettings.update(*pUpdatedSettings);
				mCurrentSettings.save();
			}
			else
			{
				qCDebug(update) << QStringLiteral("No need for replacement, current %1 is up to date").arg(mNameForLog);
			}
		}


	public:
		UpdateSettingsBackend(SettingsClass& pCurrentSettings,
				const QSharedPointer<SettingsParser<SettingsClass> > pParser,
				const QString& pDefaultSettingsFilename,
				const QString& pNameForLog)
			: UpdateBackend()
			, mCurrentSettings(pCurrentSettings)
			, mParser(pParser)
			, mDefaultSettingsFilename(pDefaultSettingsFilename)
			, mNameForLog(pNameForLog)
		{
			// (re-)load current settings to have the up-to-date issue date.
			mCurrentSettings.load();
			qCDebug(update) << QStringLiteral("Loaded current %1 issued on %2").arg(mNameForLog, mCurrentSettings.getIssueDate().toString(Qt::DateFormat::ISODate));

			// Check if more recent default settings have been installed.
			QSharedPointer<SettingsClass> defaultSettings = loadDefaultSettings();
			if (defaultSettings != nullptr && defaultSettings->getIssueDate() > mCurrentSettings.getIssueDate())
			{
				qCDebug(update) << QStringLiteral("Replacing current %1 with default %1 issued on %2").arg(mNameForLog, defaultSettings->getIssueDate().toString(Qt::DateFormat::ISODate));
				mCurrentSettings.update(*defaultSettings);
				mCurrentSettings.save();
			}
		}


		virtual ~UpdateSettingsBackend()
		{
		}


		virtual QDateTime getIssueDate() const override
		{
			return mCurrentSettings.getIssueDate();
		}


		virtual void processSuccess(const QByteArray& pData) override
		{
			QSharedPointer<SettingsClass> updatedSettings = mParser->parse(pData);
			if (updatedSettings.isNull())
			{
				qCCritical(update) << QStringLiteral("Parsing of %1 from update site failed").arg(mNameForLog);
				saveLatestSettings(QSharedPointer<SettingsClass>());
			}
			else
			{
				qCDebug(update) << QStringLiteral("Downloaded %1 from update site issued on %2").arg(mNameForLog, updatedSettings->getIssueDate().toString(Qt::DateFormat::ISODate));
				saveLatestSettings(updatedSettings);
			}
		}


		virtual void processError(const GlobalStatus& pError) override
		{
			Q_UNUSED(pError)
			saveLatestSettings(QSharedPointer<SettingsClass>());
		}


};

}
