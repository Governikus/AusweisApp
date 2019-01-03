/*!
 * \brief Loader to initialize UIPlugIns.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "UIPlugIn.h"

#include <QJsonObject>
#include <QMap>
#include <QVector>

namespace governikus
{

defineEnumType(UIPlugInName, UIPlugInQml, UIPlugInCli, UIPlugInWidgets, UIPlugInJsonApi, UIPlugInWebSocket, UIPlugInAidl)

class UILoader
	: public QObject
{
	Q_OBJECT

	private:
		QMap<UIPlugInName, UIPlugIn*> mLoadedPlugIns;
		QVector<UIPlugInName> mDefault;

		inline QString getName(UIPlugInName pPlugin) const;
		inline bool isPlugIn(const QJsonObject& pJson);
		inline bool hasName(const QJsonObject& pJson, const QString& pName);

	protected:
		UILoader();
		virtual ~UILoader();

	public:
		static UILoader& getInstance();

		bool load();
		bool load(UIPlugInName pName);

		const QStringList getDefault() const;
		void setDefault(const QStringList& pDefault);

		UIPlugIn* getLoaded(UIPlugInName pName) const;

		Q_INVOKABLE void shutdown();

	Q_SIGNALS:
		void fireLoadedPlugin(UIPlugIn* pPlugin);
		void fireShutdownComplete();
};

} // namespace governikus
