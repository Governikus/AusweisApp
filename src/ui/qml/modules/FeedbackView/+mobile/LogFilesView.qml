/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	signal logFilesListItemClicked(int index)

	//: MOBILE
	title: qsTr("Select Log")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}
	rightTitleBarAction: LogTitleBarControls {
		showRemove: LogFilesModel.count > 1

		onRemoveAllClicked: confirmationPopup.open()
	}

	GOptionsContainer {
		Layout.fillWidth: true

		Repeater {
			id: repeater

			model: LogFilesModel

			delegate: ColumnLayout {
				required property int index
				required property string modelData

				spacing: 0

				GMenuItem {
					Layout.fillWidth: true
					drawBottomCorners: parent.index == repeater.count - 1
					drawTopCorners: parent.index == 0
					title: parent.modelData

					onClicked: root.logFilesListItemClicked(parent.index)
				}
				GSeparator {
					Layout.fillWidth: true
					Layout.leftMargin: Style.dimens.pane_spacing
					Layout.rightMargin: Layout.leftMargin
					visible: parent.index != repeater.count - 1
				}
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup

		//: MOBILE
		okButtonText: qsTr("Delete")
		//: MOBILE All logfiles are about to be removed, user confirmation required.
		text: qsTr("All old logs will be deleted.")
		//: MOBILE
		title: qsTr("Delete all logs")

		onConfirmed: LogFilesModel.removeOtherLogFiles()
	}
}
