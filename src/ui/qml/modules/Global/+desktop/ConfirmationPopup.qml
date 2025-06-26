/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

BaseConfirmationPopup {
	id: root

	buttons: RowLayout {
		spacing: 0
		width: parent.width

		GSpacer {
			Layout.fillWidth: true
		}
		GButton {
			style: root.style & ConfirmationPopup.PopupStyle.OkButton ? Style.color.controlOptional : Style.color.control
			text: root.cancelButtonText
			visible: root.style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
		GButton {
			Layout.leftMargin: root.style & ConfirmationPopup.PopupStyle.CancelButton ? Style.dimens.pane_spacing : 0
			text: root.okButtonText
			visible: root.style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
	}
}
