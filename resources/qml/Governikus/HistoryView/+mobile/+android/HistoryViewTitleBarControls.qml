/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Row {
	id: baseItem

	property alias showDeleteAll: deleteEntriesButtonImage.visible

	height: Style.dimens.titlebar_height

	spacing: Constants.component_spacing

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}

	TintableIcon {
		id: deleteEntriesButtonImage

		anchors.verticalCenter: parent.verticalCenter
		height: Style.dimens.small_icon_size

		source: "qrc:///images/material_delete.svg"
		sourceSize.height: height
		tintColor: Style.color.button_text

		MouseArea {
			anchors.fill: parent

			//: LABEL ANDROID
			Accessible.name: qsTr("Delete all entries")
			Accessible.role: Accessible.Button
			Accessible.onPressAction: clicked(null)
			onClicked: deleteHistoryConfirmationPopup.open()
		}
	}
 }
