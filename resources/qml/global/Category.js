var CATEGORY_COLOR_ALL = "#659bcd"
var CATEGORY_COLOR_CITIZEN = "#aa4079"
var CATEGORY_COLOR_INSURANCE = "#52539f"
var CATEGORY_COLOR_FINANCE = "#ecc758"
var CATEGORY_COLOR_OTHER = "#00868e"

function displayString(cat) {
	if (cat === "all") {
		return qsTr("Provider")
	}
	if (cat === "citizen") {
		return qsTr("Citizen services")
	}
	if (cat === "insurance") {
		return qsTr("Insurances")
	}
	if (cat === "finance") {
		return qsTr("Financials")
	}
	return qsTr("Other services")
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
	"all": "General",
	"citizen": "CitizenServices",
	"insurance": "Insurances",
	"finance": "Financials"
}


function imageName(cat) {
	return cat in CATEGORY_TO_IMAGE_NAME ? CATEGORY_TO_IMAGE_NAME[cat] : "OtherServices"
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
	return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + "_bg.png"
}


function buttonImageSource(cat) {
	return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + "_button.png"
}


function imageSource(cat) {
	return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + ".png"
}


function sectionImageSource(cat) {
	return "qrc:///images/provider/categoryIcons/" + getPlatform() + imageName(cat) + "_section.png"
}
