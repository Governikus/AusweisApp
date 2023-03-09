/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

BaseConfirmationPopup {
	id: root
	buttons: Row {
		bottomPadding: Constants.pane_padding
		layoutDirection: Qt.RightToLeft
		rightPadding: Constants.pane_padding
		spacing: Constants.component_spacing
		width: parent.width

		GButton {
			text: root.okButtonText
			visible: style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
		GButton {
			text: root.cancelButtonText
			visible: style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
	}
}
