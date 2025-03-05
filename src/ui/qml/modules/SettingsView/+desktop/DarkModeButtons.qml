/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Type

ColumnLayout {
	id: root

	readonly property var checkedButton: system.checked ? system : dark.checked ? dark : light
	readonly property string selectedIconPath: checkedButton.icon.source
	readonly property string selectedText: checkedButton.text

	signal buttonClicked

	function onChanged(checked, mode) {
		if (!checked || SettingsModel.userDarkMode === mode)
			return;
		SettingsModel.userDarkMode = mode;
		root.buttonClicked();
	}

	spacing: 0

	Component.onCompleted: {
		if (!UiPluginModel.osDarkModeSupported)
			system.visible = false;
	}

	GRadioButton {
		id: system

		readonly property var mode: SettingsModel.ModeOption.AUTO

		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set the app appearance to system mode")
		Layout.fillWidth: true
		checked: SettingsModel.userDarkMode === mode
		icon.source: "qrc:///images/appearance_system_mode.svg"
		//: LABEL ALL_PLATFORMS
		text: qsTr("System")
		tintIcon: true

		onCheckedChanged: root.onChanged(checked, mode)
		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)
	}
	GRadioButton {
		id: dark

		readonly property var mode: SettingsModel.ModeOption.ON

		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set the app appearance to dark mode")
		Layout.fillWidth: true
		checked: SettingsModel.userDarkMode === mode
		icon.source: "qrc:///images/appearance_dark_mode.svg"
		//: LABEL ALL_PLATFORMS
		text: qsTr("Dark")
		tintIcon: true

		onCheckedChanged: root.onChanged(checked, mode)
		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)
	}
	GRadioButton {
		id: light

		readonly property var mode: SettingsModel.ModeOption.OFF

		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set the app appearance to light mode")
		Layout.fillWidth: true
		checked: SettingsModel.userDarkMode === mode
		icon.source: "qrc:///images/appearance_light_mode.svg"
		//: LABEL ALL_PLATFORMS
		text: qsTr("Light")
		tintIcon: true

		onCheckedChanged: root.onChanged(checked, mode)
		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)
	}
}
