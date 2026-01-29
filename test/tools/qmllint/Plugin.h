/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QtQmlCompiler/QtQmlCompiler>

namespace governikus
{
class Plugin
	: public QObject
	, public QQmlSA::LintPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QmlLintPluginInterface_iid FILE "metadata.json")
	Q_INTERFACES(QQmlSA::LintPlugin)

	public:
		void registerPasses(QQmlSA::PassManager* pManager, const QQmlSA::Element& pRootElement) override;
};
} // namespace governikus
