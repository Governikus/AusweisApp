import QtQuick 2.10

import Governikus.Global 1.0


Rectangle {
	readonly property int padding: Constants.pane_padding / 2

	property string providerName
	property string headerIcon
	property int nameHeight
	property string providerCategory

	width: parent.width

	Image {
		id: image
		source: parent.headerIcon !== "" ?
				parent.headerIcon :
				Category.buttonImageSource(parent.providerCategory)
		asynchronous: true
		height: parent.height
		width: height
		fillMode: Image.PreserveAspectFit
		anchors.top: parent.top
		anchors.topMargin: -parent.padding
		anchors.left: parent.left
		anchors.leftMargin: parent.padding
	}

	Text {
		text: '<html>' + providerName + '</html>'
		anchors.left: image.right
		anchors.leftMargin: parent.padding
		anchors.top: parent.top
		anchors.topMargin: parent.height * 0.05
		anchors.right: parent.right
		anchors.rightMargin: parent.padding
		elide: Text.ElideRight
		maximumLineCount: 4
		wrapMode: Text.Wrap
		lineHeightMode: Text.FixedHeight
		lineHeight: parent.height * 0.90 / 4
		font.bold: true
		font.pixelSize: Constants.small_font_size
		color: Constants.secondary_text
	}
}
