/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Type.ApplicationModel 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0

Column {
	spacing: Constants.text_spacing

	GText {
		width: parent.width

		//: INFO IOS Let user know to check the application settings for local network permission
		text: qsTr("To be able to use your smartphone as card reader (SaC), please make sure that access to the local network is allowed.")
		textStyle: Style.text.normal_secondary
	}

	MoreInformationLink {
		anchors.horizontalCenter: parent.horizontalCenter

		//: INFO IOS Link to application settings
		text: qsTr("Go to application settings")
		onClicked: ApplicationModel.showSettings(ApplicationModel.SETTING_APP)
	}
}
