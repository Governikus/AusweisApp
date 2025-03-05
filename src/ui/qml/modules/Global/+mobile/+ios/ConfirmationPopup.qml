/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global

BaseConfirmationPopup {
	id: root

	horizontalTextAlignment: Text.AlignHCenter

	buttons: ColumnLayout {
		spacing: 0
		width: parent.width

		GSeparator {
			Layout.fillWidth: true
			visible: root.style !== ConfirmationPopup.PopupStyle.NoButtons
		}
		RowLayout {
			spacing: 0

			GLink {
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				implicitWidth: 1
				text: root.cancelButtonText
				visible: root.style & ConfirmationPopup.PopupStyle.CancelButton

				onClicked: root.cancel()
			}
			GSeparator {
				Layout.fillHeight: true
				orientation: Qt.Vertical
				visible: root.style & ConfirmationPopup.PopupStyle.CancelButton && root.style & ConfirmationPopup.PopupStyle.OkButton
			}
			GLink {
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				implicitWidth: 1
				text: root.okButtonText
				visible: root.style & ConfirmationPopup.PopupStyle.OkButton

				onClicked: root.accept()
			}
		}
	}
}
