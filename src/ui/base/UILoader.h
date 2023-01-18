/*!
 * \brief Loader to initialize UIPlugIns.
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "UIPlugIn.h"

#include <QJsonObject>
#include <QMap>
#include <QStringList>


class test_UILoader;

namespace governikus
{

class UILoader
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_UILoader;

	private:
		static QStringList cUserRequest;
		QMap<QString, UIPlugIn*> mLoadedPlugIns;

		[[nodiscard]] static QStringList getInitialDefault();
		[[nodiscard]] static QString unify(const QString& pName);
		[[nodiscard]] static QString getName(const QMetaObject* pMeta);
		[[nodiscard]] static QString getName(const QJsonObject& pJson);
		[[nodiscard]] static QJsonObject getMetaDataFileContent(const QJsonObject& pJson);
		[[nodiscard]] static inline bool isDefault(const QJsonObject& pJson);
		[[nodiscard]] static inline bool isUserInteractive(const QJsonObject& pJson);
		[[nodiscard]] static QStringList getUserRequestOrDefault();
		[[nodiscard]] static inline bool isPlugIn(const QJsonObject& pJson);
		static inline void setMetaDataProperties(UIPlugIn* pUi, const QJsonObject& pJson);

#ifndef QT_NO_DEBUG

	public:
#endif
		UILoader();
		~UILoader() override;

		[[nodiscard]] bool load(const QString& pName);
		void preparePlugIn(UIPlugIn* pUi, const QJsonObject& pMetaData);

	public:
		// do not make this non-static as the CommandLineParser spawns
		// this object on startup. Since this is a QObject this should be avoided.
		[[nodiscard]] static QString getDefault();
		static void setUserRequest(const QStringList& pRequest);

		[[nodiscard]] bool initialize() const;
		[[nodiscard]] bool hasActiveUI() const;
		[[nodiscard]] bool requiresReaderManager() const;
		[[nodiscard]] bool isLoaded() const;
		[[nodiscard]] bool load();

		template<typename T>
		[[nodiscard]] std::enable_if_t<std::is_base_of_v<UIPlugIn, T>, bool> load()
		{
			return load(getName(&T::staticMetaObject));
		}


		template<typename T>
		[[nodiscard]] std::enable_if_t<std::is_base_of_v<UIPlugIn, T>, T*> getLoaded() const
		{
			return qobject_cast<T*>(mLoadedPlugIns.value(getName(&T::staticMetaObject)));
		}


		Q_INVOKABLE void shutdown();

	Q_SIGNALS:
		void fireLoadedPlugin(UIPlugIn* pPlugin);
		void fireRemovedAllPlugins();
};

} // namespace governikus
