import QtQuick 2.5

import "global"

Item {
	property string state
	property alias text: textItem.text
	property alias font: textItem.font
	readonly property alias contentWidth: root.width
	signal clicked

	id: root
	width: parent ? parent.width : undefined
	height: parent ? parent.height : undefined

	Text {
		id: textItem
		visible: false
	}
}
