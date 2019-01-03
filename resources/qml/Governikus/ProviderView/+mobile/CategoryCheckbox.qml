import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

Item {
	id: baseItem
	height: parent.height
	width: mainContent.width
	anchors.verticalCenter: parent.verticalCenter

	property string category: ""
	property alias imageSource: icon.source
	property alias text: label.text

	Row {
		id: mainContent
		height: parent.height
		spacing: Utils.dp(5)
		anchors.verticalCenter: parent.verticalCenter

		Image {
			id: icon
			height: baseItem.height * 0.7
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.verticalCenter: parent.verticalCenter
		}

		Text {
			id: label
			color: Constants.secondary_text
			font.pixelSize: Constants.normal_font_size
			anchors.verticalCenter: parent.verticalCenter
		}

		GCheckBox {
			id: checkBox
			anchors.verticalCenter: parent.verticalCenter
			visible: true
			checked: ProviderCategoryFilterModel.categories.indexOf(baseItem.category) !== -1
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: ProviderCategoryFilterModel.updateCategorySelection(category, !checkBox.checked)
	}
}
