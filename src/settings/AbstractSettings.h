/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>
#include <QSettings>
#include <QSharedPointer>
#include <QVariant>

#ifndef QT_NO_DEBUG
#include <QTemporaryDir>
#endif

namespace governikus
{

class AbstractSettings
	: public QObject
{
	Q_OBJECT

	protected:
		AbstractSettings();
		virtual ~AbstractSettings();

		static QVariant getSettingsValue(const QSharedPointer<QSettings>& pSettings, const QString& key);

	public:
#ifndef QT_NO_DEBUG
		static QSharedPointer<QTemporaryDir> mTestDir;
#endif

		static QSharedPointer<QSettings> getStore();

		virtual void load() = 0;

		virtual bool isUnsaved() const = 0;

		virtual void save() = 0;

		virtual void update(const AbstractSettings& pOther);

	Q_SIGNALS:
		void fireSettingsChanged();
};

template<typename T> bool containsAllEntries(const T& pLeftList, const T& pRightList)
{
	if (&pLeftList == &pRightList)
	{
		return true;
	}

	if (pLeftList.size() != pRightList.size())
	{
		return false;
	}

	for (const auto& entry : pLeftList)
	{
		if (!pRightList.contains(entry))
		{
			return false;
		}
	}

	for (const auto& entry : pRightList)
	{
		if (!pLeftList.contains(entry))
		{
			return false;
		}
	}

	return true;
}


} /* namespace governikus */
