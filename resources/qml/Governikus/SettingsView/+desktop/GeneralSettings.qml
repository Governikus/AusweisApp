/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {
	readonly property string helpTopic: "settingsGeneral"

	spacing: Constants.component_spacing

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Language selection")
		textStyle: Style.text.header_accent

		FocusFrame {}
	}

	LanguageButtons {
		columns: 2
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Behavior")
		textStyle: Style.text.header_accent

		FocusFrame {}
	}

	ToggleableOption {
		Layout.fillWidth: true

		activeFocusOnTab: true

		text: Qt.platform.os === "osx"
			  //: LABEL MACOS Text for auto-start option
			  ? qsTr("Auto-start %1 after boot and add to menu bar").arg(Qt.application.name)
			  //: LABEL WINDOWS Text for auto-start option
			  : qsTr("Auto-start %1 after boot").arg(Qt.application.name)
		maximumLineCount: 2
		checked: SettingsModel.autoStartApp
		enabled: !SettingsModel.autoStartSetByAdmin && SettingsModel.autoStartAvailable
		onCheckedChanged: SettingsModel.autoStartApp = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Close after authentication")
		checked: SettingsModel.autoCloseWindowAfterAuthentication
		onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Use internal notifications")
		checked: SettingsModel.showInAppNotifications
		enabled: !SettingsModel.developerMode
		onCheckedChanged: SettingsModel.showInAppNotifications = checked
	}

	GText {
		visible: SettingsModel.developerMode
		Layout.fillWidth: true

		activeFocusOnTab: true

		//: LABEL DESKTOP Only visible when the user activates the developer mode in the settings.
		text: qsTr("Using the developer mode forces the notifications to be enabled.")
		textStyle: Style.text.hint_warning

		FocusFrame {}
	}

	GSeparator {
		visible: customProxySetting.visible

		Layout.fillWidth: true
	}

	GText {
		visible: customProxySetting.visible

		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Network")
		textStyle: Style.text.header_accent

		FocusFrame {}
	}

	ToggleableOption {
		id: customProxySetting

		visible: SettingsModel.customProxyAttributesPresent

		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Use the proxy (%1) specified during the installation.").arg(SettingsModel.customProxyUrl)
		checked: SettingsModel.useCustomProxy
		onCheckedChanged: SettingsModel.useCustomProxy = checked
	}
}
