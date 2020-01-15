/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick 2.10

import Governikus.Type.Random 1.0

QtObject {

	readonly property color categoryColorNone: "#4d7aa4"
	readonly property color categoryColorAll: "#4d7aa4"
	readonly property color categoryColorCitizen: "#851e6b"
	readonly property color categoryColorInsurance: "#53428c"
	readonly property color categoryColorFinance: "#693800"
	readonly property color categoryColorOther: "#00828a"

	readonly property var categoryToColor: {
		"": categoryColorAll,
		"all": categoryColorAll,
		"citizen": categoryColorCitizen,
		"insurance": categoryColorInsurance,
		"finance": categoryColorFinance,
		"other": categoryColorOther
	}

	readonly property var categoryToImageName: {
		"citizen": "citizen",
		"insurance": "insurance",
		"finance": "finance",
		"other": "other",
		"all": "general",
		"": "default"
	}

	function getTableValue(table, key, defaultValue) {
		return key in table ? table[key] : defaultValue
	}

	function displayString(cat) {
		let categoryToDisplayString = {
			"": qsTr("Provider"),
			"all": qsTr("All"),
			"citizen": qsTr("Citizen services"),
			"insurance": qsTr("Insurances"),
			"finance": qsTr("Financials"),
			"other": qsTr("Other services")
		}
		return getTableValue(categoryToDisplayString, cat, "")
	}

	function displayColor(cat) {
		return getTableValue(categoryToColor, cat, categoryColorNone)
	}

	function imageName(cat) {
		return getTableValue(categoryToImageName, cat, "default")
	}


	function getPlatform() {
		return plugin.platformStyle.indexOf("android") !== -1 ? "+android/" : ""
	}


	function backgroundImageSource(cat) {
		return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + "_bg.svg"
	}


	function buttonImageSource(cat) {
		return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + "_button.svg"
	}


	function imageSource(cat) {
		return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + ".svg"
	}
}
