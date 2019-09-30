/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property int totalHits: ProviderCategoryFilterModel.additionalResultCount
	signal clicked()

	implicitHeight: row.implicitHeight + 2 * Constants.groupbox_spacing

	Keys.onSpacePressed: mouseArea.clicked(undefined)
	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Additional results in other categories: %1. Click here to remove filter.").arg(totalHits) + SettingsModel.translationTrigger

	Item {
		id: shadowLayer

		anchors.fill: parent

		layer.enabled: true
		layer.effect: DropShadow {
			radius: 4
			samples: 8
			source: background
			color: Qt.darker(Constants.grey, 1.2)
		}
	}

	Rectangle {
		id: background

		anchors.fill: parent

		color: Category.displayColor("all")
		radius: Style.dimens.corner_radius
	}

	RowLayout {
		id: row

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.groupbox_spacing

		spacing: Constants.groupbox_spacing

		Image {
			id: icon

			sourceSize.height: ApplicationModel.scaleFactor * 60
			sourceSize.width: ApplicationModel.scaleFactor * 60

			source: Category.imageSource("all")
			asynchronous: true
			fillMode: Image.PreserveAspectFit
		}

		GText {
			id: nameText

			verticalAlignment: Text.AlignVCenter
			Layout.fillWidth: true

			//: LABEL DESKTOP_QML
			text: qsTr("Additional results in other categories:") + " " + baseItem.totalHits + SettingsModel.translationTrigger

			textStyle: Style.text.normal
		}

		GButton {
			id: showButton

			text: qsTr("Show") + SettingsModel.translationTrigger
			onClicked: baseItem.clicked()
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}

	FocusFrame {}
}
