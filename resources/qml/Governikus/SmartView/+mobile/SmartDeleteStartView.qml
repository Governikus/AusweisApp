/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
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

	property alias buttonText: deleteButton.text
	property alias deleteDescriptionText: deleteDescription.text

	signal deleteConfirmed

	sectionPageFlickable: contentItem

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	ConfirmationPopup {
		id: deleteConfirmation
		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")
		//: LABEL ANDROID IOS
		text: qsTr("Are you sure you want to delete the Smart-eID?")

		//: LABEL ANDROID IOS
		title: qsTr("Delete Smart-eID")

		onConfirmed: root.deleteConfirmed()
	}
	GFlickableColumnLayout {
		id: contentItem

		readonly property var maxIconHeight: Style.dimens.header_icon_size
		readonly property var minIconHeight: Style.dimens.medium_icon_size

		anchors.fill: parent
		maximumContentWidth: Style.dimens.max_text_width
		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight
			source: "qrc:///images/identify.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true

			GText {
				id: deleteDescription
				width: parent.width
			}
			GText {

				//: LABEL ANDROID IOS
				text: qsTr("If you want to use that functionality again, you need to set up a new Smart-eID first.")
				width: parent.width
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			id: deleteButton
			Layout.alignment: Qt.AlignHCenter
			buttonColor: Style.color.warning_text
			icon.source: "qrc:///images/identify.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Reset Smart-eID")

			onClicked: deleteConfirmation.open()
		}
	}
}
