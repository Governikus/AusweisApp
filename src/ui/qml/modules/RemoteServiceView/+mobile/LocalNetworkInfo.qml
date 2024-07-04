/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Type
import Governikus.Global
import Governikus.Style

ColumnLayout {
	spacing: Constants.text_spacing

	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: INFO IOS Let user know to check the application settings for local network permission
		text: qsTr("Ensure that access to the local network is allowed in your settings.")
	}
	MoreInformationLink {
		Layout.alignment: Qt.AlignHCenter
		iconVisible: false
		//: INFO IOS Link to application settings
		text: qsTr("Go to application settings")

		onClicked: ApplicationModel.showSettings(ApplicationModel.Settings.APP)
	}
}
