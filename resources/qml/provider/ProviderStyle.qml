import QtQuick 2.5

import "../global"

Item {
	// ProviderViewHeader properties for ProviderView
	readonly property bool showSearchBar: true
	readonly property bool showHeaderButtonsInProviderView: false
	readonly property string headerBackgroundInProviderView: ""

	// Provider category list properties
	readonly property int categoryFontPixelSize: Utils.sp(14)
	readonly property bool categoryFontBold: false
	readonly property color categoryColor: Constants.accent_color
	readonly property int leftIconMargin: Utils.dp(0)
	readonly property int leftProviderListMargin: Utils.dp(0)
	readonly property bool showCategoryRightArrow: true

	// Provider list item properties
	readonly property int itemLeftMargin: Utils.dp(5)
	readonly property color subjectTextColor: "#000000" // default color, which is?
	readonly property bool subjectTextFontBold: false
	readonly property int addressTextFontSize: Utils.dp(11)
	readonly property color addressTextColor: Constants.blue_dark
	readonly property double infoItemWidthFactor: 2.0

	readonly property int providerListItemTopMargin: Utils.dp(2)
	readonly property int providerListItemRightMargin: Utils.dp(5)
	readonly property int providerListItemBottomMargin: Utils.dp(5)

	readonly property bool providerListItemsHaveBorder: true

	readonly property bool providerListDetailsLinkBold: false
	readonly property color providerListDetailsLinkBorder: Constants.blue_dark
	readonly property color providerListDetailsLinkColor: Constants.blue_dark
	readonly property color providerListDetailsLinkBackground: "#ffffff"
	readonly property string providerListDetailsLinkPosition: "middle"
}
