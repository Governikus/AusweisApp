/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

Item {
	id: baseItem

	property string category: ""
	property alias imageSource: icon.source
	property alias text: label.text

	height: parent.height
	width: mainContent.width
	anchors.verticalCenter: parent.verticalCenter

	Accessible.role: Accessible.CheckBox
	Accessible.name: text
	Accessible.checkable: true
	Accessible.checked: checkBox.checked
	Accessible.onPressAction: if (Qt.platform.os === "ios") mouseArea.clicked(null)

	Row {
		id: mainContent
		height: parent.height
		spacing: 5
		anchors.verticalCenter: parent.verticalCenter

		Image {
			id: icon
			height: baseItem.height * 0.7
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.verticalCenter: parent.verticalCenter
		}

		GText {
			id: label
			anchors.verticalCenter: parent.verticalCenter

			Accessible.ignored: true

			textStyle: Style.text.normal
		}

		GCheckBox {
			id: checkBox

			visible: true
			anchors.verticalCenter: parent.verticalCenter

			Accessible.ignored: true

			checked: ProviderCategoryFilterModel.categories.indexOf(baseItem.category) !== -1
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: ProviderCategoryFilterModel.updateCategorySelection(category, !checkBox.checked)
	}
}
