import QtQuick 2.7

import "global"

Text {
	id: textItem
	anchors.verticalCenter: parent.verticalCenter
	width: parent.width
	color: "white"
	font.pixelSize: Constants.titlebar_font_size
	maximumLineCount: 1
	elide: Text.ElideRight
}
