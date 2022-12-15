import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0

GridLayout {
	id: root
	signal buttonClicked

	columnSpacing: Constants.component_spacing
	rowSpacing: Constants.component_spacing

	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to german")
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("German")
		image: "qrc:///images/location_flag_de.svg"
		language: "de"
		text: "Deutsch"

		onClicked: root.buttonClicked()
	}
	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to english")
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("English")
		image: "qrc:///images/location_flag_en.svg"
		language: "en"
		text: "English"

		onClicked: root.buttonClicked()
	}
	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to ukrainian")
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("Ukrainian")
		image: "qrc:///images/location_flag_uk.svg"
		language: "uk"
		text: "Українська"

		onClicked: root.buttonClicked()
	}
	LocationButton {
		//: LABEL ALL_PLATFORMS
		Accessible.description: qsTr("Set language to russian")
		//: LABEL ALL_PLATFORMS
		Accessible.name: qsTr("Russian")
		image: "qrc:///images/location_flag_ru.svg"
		language: "ru"
		text: "Русский"

		onClicked: root.buttonClicked()
	}
}
