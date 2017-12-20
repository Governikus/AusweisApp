import QtQuick 2.7

import Governikus.Global 1.0

Text {
	id: textItem
	anchors.verticalCenter: parent.verticalCenter
	color: "white"
	font.pixelSize: Constants.titlebar_font_size
	maximumLineCount: 1
	elide: Text.ElideRight
}
