/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

ListModel {
	ListElement {
		//: LABEL ALL_PLATFORMS
		a11yDescription: qsTr("Set language to german")
		//: LABEL ALL_PLATFORMS
		a11yName: qsTr("German")
		image: "qrc:///images/location_flag_de.svg"
		language: "de"
		languageText: "Deutsch"
	}
	ListElement {
		//: LABEL ALL_PLATFORMS
		a11yDescription: qsTr("Set language to english")
		//: LABEL ALL_PLATFORMS
		a11yName: qsTr("English")
		image: "qrc:///images/location_flag_en.svg"
		language: "en"
		languageText: "English"
	}
	ListElement {
		//: LABEL ALL_PLATFORMS
		a11yDescription: qsTr("Set language to ukrainian")
		//: LABEL ALL_PLATFORMS
		a11yName: qsTr("Ukrainian")
		image: "qrc:///images/location_flag_uk.svg"
		language: "uk"
		languageText: "Українська"
	}
	ListElement {
		//: LABEL ALL_PLATFORMS
		a11yDescription: qsTr("Set language to russian")
		//: LABEL ALL_PLATFORMS
		a11yName: qsTr("Russian")
		image: "qrc:///images/location_flag_ru.svg"
		language: "ru"
		languageText: "Русский"
	}
}
