var CATEGORY_COLOR_ALL = "#659bcd"
var CATEGORY_COLOR_CITIZEN = "#aa4079"
var CATEGORY_COLOR_INSURANCE = "#52539f"
var CATEGORY_COLOR_FINANCE = "#ecc758"
var CATEGORY_COLOR_OTHER = "#00868e"

function displayString(cat) {
	if (cat === "all") {
		return qsTr("Provider") + settingsModel.translationTrigger
	}
	if (cat === "citizen") {
		return qsTr("Citizen services") + settingsModel.translationTrigger
	}
	if (cat === "insurance") {
		return qsTr("Insurances") + settingsModel.translationTrigger
	}
	if (cat === "finance") {
		return qsTr("Financials") + settingsModel.translationTrigger
	}
	return qsTr("Other services") + settingsModel.translationTrigger
}

function displayColor(cat) {
	if (cat === "all") {
		return CATEGORY_COLOR_ALL
	}
	if (cat === "citizen") {
		return CATEGORY_COLOR_CITIZEN
	}
	if (cat === "insurance") {
		return CATEGORY_COLOR_INSURANCE
	}
	if (cat === "finance") {
		return CATEGORY_COLOR_FINANCE
	}
	return CATEGORY_COLOR_OTHER
}


var CATEGORY_TO_IMAGE_NAME = {
	"all": "general",
	"citizen": "citizen",
	"insurance": "insurance",
	"finance": "finance"
}


function imageName(cat) {
	return cat in CATEGORY_TO_IMAGE_NAME ? CATEGORY_TO_IMAGE_NAME[cat] : "OtherServices"
}


function getPlatform() {
//	return plugin.platformStyle.indexOf("android") !== -1 ? "+android/" : ""
	return "+android/"
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
