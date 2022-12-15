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
		bottomPadding: Constants.groupbox_spacing / 2
		layoutDirection: Qt.RightToLeft
		rightPadding: Constants.groupbox_spacing / 2
		spacing: 0
		width: parent.width

		GButton {
			buttonColor: Style.color.transparent
			text: root.okButtonText
			textStyle: Style.text.normal_accent
			visible: style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
		GButton {
			buttonColor: Style.color.transparent
			text: root.cancelButtonText
			textStyle: Style.text.normal_accent
			visible: style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
	}
}
