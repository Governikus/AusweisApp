/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ProxyCredentials.h"
#include "UiPlugin.h"

#include <QQmlEngine>
#include <QSize>
#include <QString>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

namespace governikus
{

class UiPluginModel
	: public UiPlugin
{
	Q_OBJECT
	Q_INTERFACES(governikus::UiPlugin)
	QML_ELEMENT
	QML_SINGLETON

	Q_PROPERTY(bool debugBuild READ isDebugBuild CONSTANT)
	Q_PROPERTY(bool developerVersion READ isDeveloperVersion CONSTANT)
	Q_PROPERTY(QString dominator READ getDominator NOTIFY fireDominatorChanged)
	Q_PROPERTY(bool dominated READ isDominated NOTIFY fireDominatorChanged)
	Q_PROPERTY(QVariantMap safeAreaMargins READ getSafeAreaMargins NOTIFY fireSafeAreaMarginsChanged)
	Q_PROPERTY(bool highContrastEnabled READ isHighContrastEnabled NOTIFY fireHighContrastEnabledChanged)
	Q_PROPERTY(bool osDarkModeEnabled READ isOsDarkModeEnabled NOTIFY fireOsDarkModeChanged)
	Q_PROPERTY(bool osDarkModeSupported READ isOsDarkModeSupported CONSTANT)
	Q_PROPERTY(QString fixedFontFamily READ getFixedFontFamily CONSTANT)
	Q_PROPERTY(QSize initialWindowSize READ getInitialWindowSize CONSTANT)
	Q_PROPERTY(bool showFocusIndicator READ getShowFocusIndicator NOTIFY fireShowFocusIndicator)
	Q_PROPERTY(qreal scaleFactor READ getScaleFactor WRITE setScaleFactor NOTIFY fireScaleFactorChanged)
	Q_PROPERTY(qreal fontScaleFactor READ getFontScaleFactor NOTIFY fireFontScaleFactorChanged)
	Q_PROPERTY(bool isChromeOS READ isChromeOS CONSTANT)

	protected:
		UiPluginModel();
		~UiPluginModel() override = default;

	public:
		static UiPluginModel* create(const QQmlEngine*, const QJSEngine*);

		[[nodiscard]] virtual bool isDebugBuild() const = 0;
		[[nodiscard]] virtual bool isDeveloperVersion() const = 0;
		[[nodiscard]] virtual QString getDominator() const = 0;
		[[nodiscard]] virtual bool isDominated() const = 0;
		[[nodiscard]] virtual QVariantMap getSafeAreaMargins() const = 0;
		[[nodiscard]] virtual bool isHighContrastEnabled() const = 0;
		[[nodiscard]] virtual bool isOsDarkModeEnabled() const = 0;
		[[nodiscard]] virtual bool isOsDarkModeSupported() const = 0;
		[[nodiscard]] virtual QString getFixedFontFamily() const = 0;
		[[nodiscard]] virtual QSize getInitialWindowSize() const = 0;
		[[nodiscard]] virtual bool getShowFocusIndicator() const = 0;
		[[nodiscard]] virtual qreal getScaleFactor() const = 0;
		virtual void setScaleFactor(qreal pScaleFactor) = 0;
		[[nodiscard]] virtual qreal getFontScaleFactor() const = 0;
		[[nodiscard]] virtual bool isChromeOS() const = 0;

		Q_INVOKABLE virtual void hideFromTaskbar() const = 0;
		Q_INVOKABLE virtual void doRefresh() = 0;

	Q_SIGNALS:
		void fireShowRequest(UiModule pModule);
		void fireHideRequest();
		void fireDominatorChanged();
		void fireSafeAreaMarginsChanged();
		void fireHighContrastEnabledChanged();
		void fireOsDarkModeChanged();
		void fireProxyAuthenticationRequired(ProxyCredentials* pProxyCredentials);
		void fireShowFocusIndicator();
		void fireScaleFactorChanged();
		void fireFontScaleFactorChanged();
};

} // namespace governikus
