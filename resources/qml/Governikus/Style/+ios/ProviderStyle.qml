import QtQuick 2.5

import Governikus.Global 1.0

Item {
	// Provider category list properties
	readonly property int categoryFontPixelSize: Constants.normal_font_size
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
	readonly property color addressTextColor: PlatformConstants.blue_dark
	readonly property double infoItemWidthFactor: 2.0

	readonly property int providerListItemTopMargin: Utils.dp(2)
	readonly property int providerListItemRightMargin: Utils.dp(5)
	readonly property int providerListItemBottomMargin: Utils.dp(5)

	readonly property bool providerListItemsHaveBorder: true

	readonly property bool providerListDetailsLinkBold: false
	readonly property color providerListDetailsLinkBorder: PlatformConstants.blue_dark
	readonly property color providerListDetailsLinkColor: PlatformConstants.blue_dark
	readonly property color providerListDetailsLinkBackground: "#ffffff"
	readonly property string providerListDetailsLinkPosition: "middle"
}
