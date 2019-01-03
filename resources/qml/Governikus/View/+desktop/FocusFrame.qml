import QtQuick 2.10

import Governikus.Global 1.0


Rectangle {
	property Item framee: parent
	property Item scope: parent
	property real marginFactor: 1

	readonly property real size: Math.max(Math.min(framee.width, framee.height) / 32, 1)

	id: border
	anchors.fill: framee
	anchors.margins: marginFactor * -size * 2
	radius: size * 2
	border.width: scope.focus ? size : 0;
	border.color: Constants.white
	opacity: 0.5
	color: "transparent"
}
