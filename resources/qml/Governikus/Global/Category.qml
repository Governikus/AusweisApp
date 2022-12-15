/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick 2.15

QtObject {
	readonly property color categoryColorAll: "#164a8c"
	readonly property color categoryColorCitizen: "#851e6b"
	readonly property color categoryColorFinance: "#693800"
	readonly property color categoryColorInsurance: "#53428c"
	readonly property color categoryColorNone: "#164a8c"
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

	function backgroundImageSource(cat) {
		return "qrc:///images/provider/" + getPlatform() + imageName(cat) + "_bg.svg";
	}
	function buttonImageSource(cat) {
		return "qrc:///images/provider/" + getPlatform() + imageName(cat) + "_button.svg";
	}
	function displayColor(cat) {
		return getTableValue(categoryToColor, cat, categoryColorNone);
	}
	function displayString(cat) {
		let categoryToDisplayString = {
			"": qsTr("Provider"),
			"all": qsTr("All"),
			"citizen": qsTr("Citizen services"),
			"insurance": qsTr("Insurances"),
			"finance": qsTr("Financials"),
			"other": qsTr("Other services")
		};
		return getTableValue(categoryToDisplayString, cat, "");
	}
	function getPlatform() {
		return plugin.platformStyle.indexOf("ios") !== -1 ? "ios/" : "";
	}
	function getTableValue(table, key, defaultValue) {
		return key in table ? table[key] : defaultValue;
	}
	function imageName(cat) {
		return getTableValue(categoryToImageName, cat, "default");
	}
	function imageSource(cat) {
		return "qrc:///images/provider/" + getPlatform() + imageName(cat) + ".svg";
	}
}
