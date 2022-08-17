/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: root

	signal deleteConfirmed

	property alias buttonText: deleteButton.text
	property alias deleteDescriptionText: deleteDescription.text

	ConfirmationPopup {
		id: deleteConfirmation

		//: LABEL ANDROID IOS
		title: qsTr("Delete Smart-eID")
		//: LABEL ANDROID IOS
		text: qsTr("Are you sure you want to delete the Smart-eID?")
		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")

		onConfirmed: root.deleteConfirmed()
	}

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	sectionPageFlickable: contentItem
	GFlickableColumnLayout {
		id: contentItem

		readonly property var minIconHeight: Style.dimens.medium_icon_size
		readonly property var maxIconHeight: Style.dimens.header_icon_size

		anchors.fill: parent

		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		maximumContentWidth: Style.dimens.max_text_width
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.fillHeight: true
			Layout.alignment: Qt.AlignHCenter
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight

			source: "qrc:///images/identify.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent
		}

		GPane {
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			GText {
				id: deleteDescription

				width: parent.width
			}

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("If you want to use that functionality again, you need to set up a new Smart-eID first.")
			}
		}

		GSpacer { Layout.fillHeight: true }

		GButton {
			id: deleteButton

			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Reset Smart-eID")
			icon.source: "qrc:///images/identify.svg"
			buttonColor: Style.color.warning_text

			onClicked: deleteConfirmation.open()
		}
	}
}
