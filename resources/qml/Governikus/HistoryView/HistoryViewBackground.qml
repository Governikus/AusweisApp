import QtQuick 2.5

import Governikus.Global 1.0

Rectangle {
	id: redLine
	height: parent.height
	width: Utils.dp(2)

	anchors.left: parent.left
	anchors.leftMargin: Utils.dp(30)

	color: "red"
	opacity: 0.05
}
