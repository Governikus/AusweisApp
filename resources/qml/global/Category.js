function getTableValue(table, key, defaultValue) {
	return key in table ? table[key] : defaultValue
}


var CATEGORY_TO_DISPLAY_STRING = {
	"": qsTr("Provider"),
	"all": qsTr("All"),
	"citizen": qsTr("Citizen services"),
	"insurance": qsTr("Insurances"),
	"finance": qsTr("Financials"),
	"other":  qsTr("Other services")
}

function displayString(cat) {
	return getTableValue(CATEGORY_TO_DISPLAY_STRING, cat, "")
}


var CATEGORY_COLOR_NONE = "#659bcd"
var CATEGORY_COLOR_ALL = "#659bcd"
var CATEGORY_COLOR_CITIZEN = "#aa4079"
var CATEGORY_COLOR_INSURANCE = "#52539f"
var CATEGORY_COLOR_FINANCE = "#ecc758"
var CATEGORY_COLOR_OTHER = "#00868e"

var CATEGORY_TO_COLOR = {
	"": CATEGORY_COLOR_ALL,
	"all": CATEGORY_COLOR_ALL,
	"citizen": CATEGORY_COLOR_CITIZEN,
	"insurance": CATEGORY_COLOR_INSURANCE,
	"finance": CATEGORY_COLOR_FINANCE,
	"other": CATEGORY_COLOR_OTHER
}

function displayColor(cat) {
	return getTableValue(CATEGORY_TO_COLOR, cat, CATEGORY_COLOR_NONE)
}


var CATEGORY_TO_IMAGE_NAME = {
	"citizen": "CitizenServices",
	"insurance": "Insurances",
	"finance": "Financials",
	"other": "OtherServices",
	"all": "General",
	"": "General"
}

function imageName(cat) {
	return getTableValue(CATEGORY_TO_IMAGE_NAME, cat, "General")
}


function getPlatform() {
	return plugin.platformStyle.indexOf("android") !== -1 ? "+android/" : ""
}


function gradientImageSource(cat) {
	if (cat !== "citizen" && cat !== "insurance" && cat !== "finance") {
		return "qrc:///images/provider/gradient-other.png"
	}
	else {
		return "qrc:///images/provider/gradient-" + cat + ".png"
	}
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


function sectionImageSource(cat) {
	return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + "_section.svg"
}
