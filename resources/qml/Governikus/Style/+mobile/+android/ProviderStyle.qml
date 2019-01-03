pragma Singleton

import QtQuick 2.10

import Governikus.Global 1.0

Item {
	// Provider category list properties
	readonly property int categoryFontPixelSize: Constants.titlebar_font_size
	readonly property bool categoryFontBold: true
	readonly property color categoryColor: Constants.accent_color
	readonly property int leftIconMargin: Utils.dp(10)
	readonly property int leftProviderListMargin: Utils.dp(20)
	readonly property bool showCategoryRightArrow: false

	// Provider list item properties
	readonly property int itemLeftMargin: Utils.dp(15)
	readonly property color subjectTextColor: Constants.secondary_text
	readonly property bool subjectTextFontBold: true
	readonly property int addressTextFontSize: Utils.dp(12)
	readonly property color addressTextColor: Constants.accent_color
	readonly property double infoItemWidthFactor: 4.0

	readonly property int providerListItemTopMargin: Utils.dp(0)
	readonly property int providerListItemRightMargin: Utils.dp(0)
	readonly property int providerListItemBottomMargin: Utils.dp(0)

	readonly property bool providerListItemsHaveBorder: true

	readonly property bool providerListDetailsLinkBold: true
	readonly property color providerListDetailsLinkBorder: Constants.grey_light
	readonly property color providerListDetailsLinkColor: Constants.primary_text
	readonly property color providerListDetailsLinkBackground: Constants.grey_light
	readonly property string providerListDetailsLinkPosition: "top"
}
