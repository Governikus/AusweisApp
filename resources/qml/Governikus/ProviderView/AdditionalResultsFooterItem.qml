/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property int totalHits: ProviderCategoryFilterModel.additionalResultCount

	signal clicked

	Accessible.name: qsTr("Additional results in other categories: %1. Click here to remove filter.").arg(totalHits)
	Accessible.role: Accessible.Button
	implicitHeight: row.implicitHeight + 2 * Constants.groupbox_spacing

	Keys.onSpacePressed: mouseArea.clicked(undefined)

	Rectangle {
		id: background
		anchors.fill: parent
		color: Category.displayColor("all")
		radius: Style.dimens.corner_radius
	}
	RowLayout {
		id: row
		anchors.fill: parent
		anchors.margins: Constants.groupbox_spacing
		spacing: Constants.groupbox_spacing

		Image {
			id: icon
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			source: Category.imageSource("all")
			sourceSize.height: Style.dimens.medium_icon_size
			sourceSize.width: Style.dimens.medium_icon_size
		}
		GText {
			id: nameText
			Layout.fillWidth: true

			//: LABEL DESKTOP IOS_TABLET ANDROID_TABLET
			text: qsTr("Additional results in other categories:") + " " + baseItem.totalHits
			textStyle: Style.text.normal_inverse
			verticalAlignment: Text.AlignVCenter
		}
		GText {
			padding: Constants.text_spacing
			text: qsTr("Show")
			textStyle: Style.text.normal_inverse
		}
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor

		onClicked: baseItem.clicked()
	}
	FocusFrame {
	}
}
