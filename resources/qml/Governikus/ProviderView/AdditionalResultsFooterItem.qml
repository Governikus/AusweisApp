/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

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
	Accessible.name: qsTr("Additional results in other categories: %1. Click here to remove filter.").arg(totalHits)

	Item {
		id: shadowLayer

		anchors.fill: parent

		layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
		layer.effect: GDropShadow { source: background }
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

			sourceSize.height: Style.dimens.medium_icon_size
			sourceSize.width: Style.dimens.medium_icon_size

			source: Category.imageSource("all")
			asynchronous: true
			fillMode: Image.PreserveAspectFit
		}

		GText {
			id: nameText

			verticalAlignment: Text.AlignVCenter
			Layout.fillWidth: true

			//: LABEL DESKTOP IOS_TABLET ANDROID_TABLET
			text: qsTr("Additional results in other categories:") + " " + baseItem.totalHits
			textStyle: Style.text.normal_inverse
		}

		GText {
			padding: Constants.text_spacing
			text: qsTr("Show")
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		cursorShape: Qt.PointingHandCursor
		onClicked: baseItem.clicked()
	}

	FocusFrame {
		borderColor: Style.color.focus_indicator
	}
}
