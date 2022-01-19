/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Type.SettingsModel 1.0

Loader {
	id: loader

	property var contentArea
	property int module
	readonly property bool isLoaded: status === Loader.Ready
	readonly property int startupModule: SettingsModel.startupModule

	anchors.fill: contentArea
	visible: contentArea.activeModule === module

	active: contentArea.initialViewsReady || contentArea.activeModule === module || startupModule === module
	asynchronous: startupModule !== module
}
