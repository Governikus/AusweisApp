/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

ConfirmationPopup {
	id: baseItem

	//: LABEL ANDROID IOS
	title: qsTr("Select language")

	style: ConfirmationPopup.PopupStyle.CancelButton

	ColumnLayout {
		spacing: Constants.component_spacing

		LocationButton {
			Accessible.name: qsTr("German")
			Accessible.description: qsTr("Set language to german")

			language: "de"
			text: "Deutsch"
			image: "qrc:///images/location_flag_de.svg"

			onClicked: close()
		}

		LocationButton {
			Accessible.name: qsTr("English")
			Accessible.description: qsTr("Set language to english")

			language: "en"
			text: "English"
			image: "qrc:///images/location_flag_en.svg"

			onClicked: close()
		}
	}
}
