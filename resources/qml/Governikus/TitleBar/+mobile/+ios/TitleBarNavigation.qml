/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

TitleBarAction {
	id: baseItem

	Accessible.role: Accessible.Button
	Accessible.name: text

	icon: state === "back" ? "qrc:///images/arrowLeft.svg" : null
	//: LABEL ANDROID IOS
	text: (state === "cancel" ? qsTr("Cancel") :
		//: LABEL ANDROID IOS
		state === "edit" ? qsTr("Edit") :
		//: LABEL ANDROID IOS
		state === "back" ? (qsTr("Back")) :
		state === "hidden" ? "" :
		"") + SettingsModel.translationTrigger
}
