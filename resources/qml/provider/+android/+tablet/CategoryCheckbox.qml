import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../../../global"

Item {
	id: baseItem

	height: parent.height
	width: mainContent.width
	anchors.verticalCenter: parent.verticalCenter

	property string category: ""
	property string imageSource: ""
	property string text: ""
	property bool checked: false

	onCheckedChanged: providerModel.categoryFilter.updateCategorySelection(category, checked)

	Connections {
		target: providerModel.categoryFilter
		onFireCriteriaChanged: {
			baseItem.checked = providerModel.categoryFilter.isSelected(category)
		}
	}

	Row {
		id: mainContent

		height: parent.height
		spacing: Utils.dp(5)
		anchors.verticalCenter: parent.verticalCenter

		Image {
			id: icon
			source: baseItem.imageSource

			fillMode: Image.PreserveAspectFit
			height: baseItem.height * 0.7
			width: height
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			text: baseItem.text
			color: "black"
			font.pixelSize: Utils.sp(16)
			anchors.verticalCenter: parent.verticalCenter
		}

		CheckBox {
			anchors.verticalCenter: parent.verticalCenter
			visible: true
			checked: baseItem.checked
		}
	}

	MouseArea {
		anchors.fill: parent
		enabled: true
		onClicked: baseItem.checked = !baseItem.checked
	}
}
