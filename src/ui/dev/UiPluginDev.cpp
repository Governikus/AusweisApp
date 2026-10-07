/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "UiPluginDev.h"

#include "Env.h"
#include "UiLoader.h"
#include "UiPluginQml.h"

#include <QDir>
#include <QLoggingCategory>
#include <QQmlComponent>


Q_DECLARE_LOGGING_CATEGORY(gui)


using namespace governikus;


UiPluginDev::UiPluginDev()
	: UiPlugin()
{
}


QString UiPluginDev::getActiveSelector() const
{
	auto platform = qEnvironmentVariable("OVERRIDE_PLATFORM");
	return platform.isEmpty() ? QStringLiteral("+desktop") : QStringLiteral("+mobile");
}


QStringList UiPluginDev::getTestFiles() const
{
	QDir root(QStringLiteral(":/dev/qml"));
	auto files = root.entryList({QStringLiteral("test_*.qml")}, QDir::Files, QDir::Name);

	auto selector = getActiveSelector();
	QDir platformRoot(root.absoluteFilePath(selector));
	auto platformFiles = platformRoot.entryList({QStringLiteral("test_*.qml")}, QDir::Files, QDir::Name);
	std::ranges::for_each(platformFiles, [selector](QString& s) {
				s.prepend(QStringLiteral("/"));
				s.prepend(selector);
			});

	return files << platformFiles;
}


bool UiPluginDev::initialize()
{
	const auto* qmlPlugin = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>();
	if (!qmlPlugin)
	{
		qCWarning(gui) << "UiPluginDev requires UiPluginQml";
		return false;
	}
	return true;
}


void UiPluginDev::doShutdown()
{
}


void UiPluginDev::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UiPluginDev::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UiPluginDev::onApplicationStarted()
{
	const auto* qmlPlugin = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>();

	auto* engine = qmlPlugin->getEngine();
	if (!engine)
	{
		qCCritical(gui) << "No QQmlApplicationEngine";
		return;
	}

	auto* appWindow = qmlPlugin->getRootWindow();
	if (!appWindow)
	{
		qCCritical(gui) << "No QQuickWindow";
		return;
	}

	const auto file = QStringLiteral("qrc:/dev/qml/TestSettingsWindow.qml");
	QQmlComponent component(engine, QUrl(file));
	if (component.isError())
	{
		qCCritical(gui).noquote() << "TestSettingsWindow load errors:" << component.errorString();
		return;
	}

	auto* object = component.beginCreate(engine->rootContext());
	if (!object)
	{
		qCCritical(gui) << "TestSettingsWindow could not be created:" << component.errorString();
		return;
	}
	object->setProperty("app", QVariant::fromValue(appWindow));
	auto files = getTestFiles();
	object->setProperty("testFiles", QVariant::fromValue(files));
	component.completeCreate();

	object->setParent(this);
}
