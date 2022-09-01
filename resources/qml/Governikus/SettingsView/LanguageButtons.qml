import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0

GridLayout {
	id: root

	signal buttonClicked()

	columnSpacing: Constants.component_spacing
	rowSpacing: Constants.component_spacing

	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("German")
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to german")

		language: "de"
		text: "Deutsch"
		image: "qrc:///images/location_flag_de.svg"
		onClicked: root.buttonClicked()
	}

	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("English")
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to english")

		language: "en"
		text: "English"
		image: "qrc:///images/location_flag_en.svg"
		onClicked: root.buttonClicked()
	}

	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("Ukrainian")
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to ukrainian")

		language: "uk"
		text: "Українська"
		image: "qrc:///images/location_flag_uk.svg"
		onClicked: root.buttonClicked()
	}

	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("Russian")
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to russian")

		language: "ru"
		text: "Русский"
		image: "qrc:///images/location_flag_ru.svg"
		onClicked: root.buttonClicked()
	}

}
