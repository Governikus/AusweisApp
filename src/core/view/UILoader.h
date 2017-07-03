/*!
 * \brief Loader to initialize UIPlugIns.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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

		inline bool isPlugIn(const QJsonObject& pJson);
		inline bool hasName(const QJsonObject& pJson, const QString& pName);

	protected:
		UILoader();
		virtual ~UILoader();

	public:
		static UILoader& getInstance();

		bool load();
		bool load(UIPlugInName pName);

		const QVector<UIPlugInName>& getDefault() const
		{
			return mDefault;
		}


		void setDefault(const QVector<UIPlugInName>& pDefault)
		{
			mDefault = pDefault;
		}


		UIPlugIn* getLoaded(UIPlugInName pName) const;

	Q_SIGNALS:
		void fireLoadedPlugin(UIPlugIn* pPlugin);
};

} /* namespace governikus */
