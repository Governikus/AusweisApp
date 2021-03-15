/*!
 * \brief Loader to initialize UIPlugIns.
 *
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "UIPlugIn.h"

#include <QJsonObject>
#include <QMap>
#include <QVector>

namespace governikus
{

defineEnumType(UIPlugInName, UIPlugInQml, UIPlugInJson, UIPlugInWebSocket, UIPlugInAidl, UIPlugInFunctional)

class UILoader
	: public QObject
{
	Q_OBJECT
	friend class Env;

	private:
		static QVector<UIPlugInName> cDefault;
		QMap<UIPlugInName, UIPlugIn*> mLoadedPlugIns;

		static QVector<UIPlugInName> getInitialDefault();
		static QString getName(UIPlugInName pPlugin);

		UILoader();
		~UILoader() override;
		[[nodiscard]] inline bool isPlugIn(const QJsonObject& pJson) const;
		[[nodiscard]] inline bool hasName(const QJsonObject& pJson, const QString& pName) const;

	public:
		// do not make this non-static as the CommandLineParser spawns
		// this object on startup. Since this is a QObject this shoud be avoided.
		static QStringList getDefault();
		static void setDefault(const QStringList& pDefault);

		[[nodiscard]] bool isLoaded() const;
		bool load();
		bool load(UIPlugInName pName);

		[[nodiscard]] UIPlugIn* getLoaded(UIPlugInName pName) const;

		Q_INVOKABLE void shutdown();

	Q_SIGNALS:
		void fireLoadedPlugin(UIPlugIn* pPlugin);
		void fireShutdownComplete();
};

} // namespace governikus
