/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	id: baseItem

	property alias showDeleteAll: deleteEntriesButtonImage.visible

	height: Style.dimens.titlebar_height

	spacing: Constants.component_spacing

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}

	TintableIcon {
		id: deleteEntriesButtonImage

		sourceSize.height: Style.dimens.small_icon_size
		sourceSize.width: Style.dimens.small_icon_size

		source: "qrc:///images/material_delete.svg"
		tintColor: Style.color.button_text

		MouseArea {
			anchors.fill: parent

			//: LABEL ANDROID
			Accessible.name: qsTr("Delete all entries")
			Accessible.role: Accessible.Button
			onClicked: deleteHistoryConfirmationPopup.open()
		}
	}
 }
