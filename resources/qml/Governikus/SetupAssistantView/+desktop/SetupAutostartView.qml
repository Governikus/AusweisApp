/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.SettingsModel
import Governikus.Type.UiModule

DecisionView {
	mainIconSource: "qrc:///images/desktop/autostart.svg"
	questionSubText: {
		//: INFO DESKTOP Information text why autostart of the App is advisable
		let subText = qsTr("In order to successfully use the eID function, %1 has to be running. It is therefore advisable to activate the auto-start after system startup.").arg(Qt.application.name);
		if (Qt.platform.os === "osx") {
			//: INFO MACOS Additional information that macOS auto-start adds a symbol to the menu bar
			subText += " " + qsTr("The launch will add an icon to the menu bar.");
		} else {
			//: INFO WINDOWS Additional information that Windows auto-start adds a symbol to the notification area.
			subText += " " + qsTr("The launch will add a tray icon to the notification area.");
		}
		return subText;
	}
	//: INFO DESKTOP Question if the App shall be started automatically after boot
	questionText: qsTr("Do you want to automatically start the %1 after boot?").arg(Qt.application.name)

	titleBarAction: TitleBarAction {
		rootEnabled: false
		showSettings: false
		//: LABEL DESKTOP
		text: qsTr("Autostart Settings")

		customSubAction: Item {
		}
	}

	onAgree: {
		SettingsModel.autoStartApp = true;
		SettingsModel.startupModule = UiModule.DEFAULT;
		nextView(UiModule.DEFAULT);
	}
	onDisagree: {
		SettingsModel.autoStartApp = false;
		SettingsModel.startupModule = UiModule.DEFAULT;
		nextView(UiModule.DEFAULT);
	}
}
