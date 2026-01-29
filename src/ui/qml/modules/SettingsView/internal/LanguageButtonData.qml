/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

ListModel {
	ListElement {
		//: ALL_PLATFORMS
		desc: qsTr("Set language to german")
		img: "qrc:///images/location_flag_de.svg"
		//: ALL_PLATFORMS
		name: qsTr("German")
		value: "de"
	}
	ListElement {
		//: ALL_PLATFORMS
		desc: qsTr("Set language to english")
		img: "qrc:///images/location_flag_en.svg"
		//: ALL_PLATFORMS
		name: qsTr("English")
		value: "en"
	}
	ListElement {
		//: ALL_PLATFORMS
		desc: qsTr("Set language to ukrainian")
		img: "qrc:///images/location_flag_uk.svg"
		//: ALL_PLATFORMS
		name: qsTr("Ukrainian")
		value: "uk"
	}
	ListElement {
		//: ALL_PLATFORMS
		desc: qsTr("Set language to russian")
		img: "qrc:///images/location_flag_ru.svg"
		//: ALL_PLATFORMS
		name: qsTr("Russian")
		value: "ru"
	}
}
