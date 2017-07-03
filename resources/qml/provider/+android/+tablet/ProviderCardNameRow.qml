import QtQuick 2.6

import "../../../global"


Rectangle {
	id: baseItem

	readonly property int nameFontSize: Constants.small_font_size

	property string headerIcon
	property int nameHeight
	property string providerCategory
	property int sidePadding

	height: baseItem.nameHeight + baseItem.sidePadding
	width: parent.width

	Row {
		id: nameRow

		height: parent.height
		width: parent.width
		anchors.centerIn: parent

		leftPadding: baseItem.sidePadding
		rightPadding: baseItem.sidePadding
		spacing: baseItem.sidePadding * 1.3333

		Image {
			source: baseItem.headerIcon !== "" ?
					baseItem.headerIcon :
					Category.buttonImageSource(baseItem.providerCategory)

			fillMode: Image.PreserveAspectFit
			height: width
			width: nameRow.width / 3 - nameRow.spacing
			anchors.top: parent.top
			anchors.topMargin: -baseItem.sidePadding
		}

		Text {
			text: '<html>' + providerName + '</html>'

			property int customLineHeight: nameRow.height * 0.90 / 4
			width: nameRow.width * 2 / 3 - baseItem.sidePadding
			anchors.top: parent.top
			elide: Text.ElideRight
			maximumLineCount: 4
			wrapMode: Text.Wrap
			lineHeightMode: Text.FixedHeight
			lineHeight: customLineHeight

			topPadding: (nameRow.height - 4 * customLineHeight) / 2
			rightPadding: baseItem.sidePadding

			font.bold: true
			font.pixelSize: baseItem.nameFontSize
			color: PlatformConstants.dark_grey_secondary_text
		}
	}
}
