/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

DecisionView {
	agreeButtonText: Qt.platform.os === "osx" ?
	//: INFO MACOS Button to decide to attach app to the menu bar on macOS
	qsTr("Yes, attach app to menu bar") :
	//: INFO WINDOWS Button to decide to attach app to the tray on Windows
	qsTr("Yes, attach app to tray")
	descriptionTextsModel: [
		//: INFO DESKTOP Information text why attaching the app to the tray/menu bar is advisable 1/3
		qsTr("The %1 continues to run in the background after the application window is closed, so that it can be automatically opened on an authentication.").arg(Qt.application.name),
		//: INFO DESKTOP Information text why attaching the app to the tray/menu bar is advisable 2/3
		"%1%2".arg(qsTr("An authentication cannot be carried out if the %1 is not running. Then the app has to be started manually before another authentication.").arg(Qt.application.name)).arg(Qt.platform.os === "osx" ?
		//: INFO MACOS Information text why attaching the app to the tray/menu bar is advisable 3/3
		qsTr("Attaching the app to the menu bar is therefore recommended.") :
		//: INFO WINDOWS Information text why attaching the app to the tray/menu bar is advisable 3/3
		qsTr("Attaching the app to the tray is therefore recommended."))]
	disagreeButtonHighlighted: false
	disagreeButtonText: Qt.platform.os === "osx" ?
	//: INFO MACOS Button to decide not to attach app to the menu bar on macOS
	qsTr("No, don't attach app to menu bar") :
	//: INFO WINDOWS Button to decide not to attach app to the tray on Windows
	qsTr("No, don't attach app to tray")
	subtitleText: Qt.platform.os === "osx" ?
	//: INFO MACOS Question if the App shall be attached to the menu bar on macOS
	qsTr("Do you want to attach the %1 to the menu bar?").arg(Qt.application.name) :
	//: INFO WINDOWS Question if the App shall be attached to the tray on windows
	qsTr("Do you want to attach the %1 to the tray?").arg(Qt.application.name)

	iconSourceComponent: TintableIcon {
		source: Qt.platform.os === "osx" ? "qrc:///images/desktop/trayicon_position_macos.svg" : "qrc:///images/desktop/trayicon_position_win.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
	}
	titleBarSettings: TitleBarSettings {
		startEnabled: false
	}

	onAgreeClicked: {
		SettingsModel.trayIconEnabled = true;
		leaveView();
	}
	onDisagreeClicked: {
		SettingsModel.trayIconEnabled = false;
		leaveView();
	}
}
