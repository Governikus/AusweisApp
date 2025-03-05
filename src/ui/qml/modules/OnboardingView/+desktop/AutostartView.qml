/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

DecisionView {
	//: INFO DESKTOP Button to decide start the app automatically
	agreeButtonText: qsTr("Yes, start app automatically")
	descriptionTextsModel: [
		//: INFO DESKTOP Information text why autostart of the App is advisable 1/2
		qsTr("The %1 gets started on system boot, so that it can be automatically opened on an authentication.").arg(Qt.application.name),
		//: INFO DESKTOP Information text why autostart of the App is advisable 2/2
		qsTr("An authentication cannot be carried out if the %1 is not running. Then it has to be started manually. The automatic start is therefore recommended.").arg(Qt.application.name)]
	disagreeButtonHighlighted: false
	//: INFO DESKTOP Button to decide to not start the app automatically
	disagreeButtonText: qsTr("No, don't start app automatically")

	//: INFO DESKTOP Question if the App shall be started automatically after boot
	subtitleText: qsTr("Do you want to automatically start the %1 after boot?").arg(Qt.application.name)

	iconSourceComponent: TintableIcon {
		source: "qrc:///images/desktop/autostart.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
	}
	titleBarSettings: TitleBarSettings {
		startEnabled: false
	}

	onAgreeClicked: {
		SettingsModel.autoStartApp = true;
		leaveView();
	}
	onDisagreeClicked: {
		SettingsModel.autoStartApp = false;
		leaveView();
	}
}
