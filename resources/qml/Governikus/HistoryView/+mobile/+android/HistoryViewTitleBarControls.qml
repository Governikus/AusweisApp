/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

RowLayout {
	id: baseItem

	property alias showDeleteAll: deleteEntriesButtonImage.visible

	height: Style.dimens.titlebar_height

	spacing: Constants.component_spacing

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}

	Image {
		id: deleteEntriesButtonImage

		sourceSize.height: 36
		sourceSize.width: 36

		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/trash_icon_white.svg"

		MouseArea {
			anchors.fill: parent

			//: LABEL ANDROID
			Accessible.name: qsTr("Delete all entries") + SettingsModel.translationTrigger
			Accessible.role: Accessible.Button
			onClicked: deleteHistoryConfirmationPopup.open()
		}
	}
 }
