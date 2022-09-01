/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0


BaseConfirmationPopup {
	id: root

	headerTextStyle: Style.text.header

	buttons: Flow {
		width: parent.width

		layoutDirection: Qt.RightToLeft
		rightPadding: Constants.groupbox_spacing / 2
		bottomPadding: Constants.groupbox_spacing / 2
		spacing: 0

		GButton {
			visible: style & ConfirmationPopup.PopupStyle.OkButton

			buttonColor: Style.color.transparent
			text: root.okButtonText
			textStyle: Style.text.normal_accent

			onClicked: root.accept()
		}

		GButton {
			visible: style & ConfirmationPopup.PopupStyle.CancelButton

			buttonColor: Style.color.transparent
			text: root.cancelButtonText
			textStyle: Style.text.normal_accent

			onClicked: root.cancel()
		}
	}
}
