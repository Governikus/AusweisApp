/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Type

ColumnLayout {
	id: root

	readonly property var checkedButton: system.mode === SettingsModel.userDarkMode ? system : dark.mode === SettingsModel.userDarkMode ? dark : light
	readonly property string selectedIconPath: checkedButton.image
	readonly property string selectedText: checkedButton.text

	signal buttonClicked

	function onAppearanceButtonClicked(mode) {
		if (SettingsModel.userDarkMode === mode)
			return;
		SettingsModel.userDarkMode = mode;
		root.buttonClicked();
	}

	spacing: 0

	Component.onCompleted: {
		if (!UiPluginModel.osDarkModeSupported)
			system.visible = false;
	}

	GCollapsibleSubButton {
		id: system

		readonly property var mode: SettingsModel.ModeOption.AUTO

		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set the app appearance to system mode")
		Layout.fillWidth: true
		image: "qrc:///images/appearance_system_mode.svg"
		//: LABEL ALL_PLATFORMS
		text: qsTr("System")
		tintIcon: true

		onClicked: root.onAppearanceButtonClicked(mode)
	}
	GCollapsibleSubButton {
		id: dark

		readonly property var mode: SettingsModel.ModeOption.ON

		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set the app appearance to dark mode")
		Layout.fillWidth: true
		image: "qrc:///images/appearance_dark_mode.svg"
		//: LABEL ALL_PLATFORMS
		text: qsTr("Dark")
		tintIcon: true

		onClicked: root.onAppearanceButtonClicked(mode)
	}
	GCollapsibleSubButton {
		id: light

		readonly property var mode: SettingsModel.ModeOption.OFF

		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set the app appearance to light mode")
		Layout.fillWidth: true
		image: "qrc:///images/appearance_light_mode.svg"
		//: LABEL ALL_PLATFORMS
		text: qsTr("Light")
		tintIcon: true

		onClicked: root.onAppearanceButtonClicked(mode)
	}
}
