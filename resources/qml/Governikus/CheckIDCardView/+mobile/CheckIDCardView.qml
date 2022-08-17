/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.CheckIDCardModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: root

	signal startAuth()

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: show(UiModule.DEFAULT) }

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	onStartAuth: {
		popAll()
		show(UiModule.SELF_AUTHENTICATION)
	}

	QtObject {
		id: d

		function restartCheck() {
			popAll()
			startCheck()
		}

		function startCheck() {
			setLockedAndHidden()
			push(checkIDCardWorkflow)
		}

		function cancel() {
			setLockedAndHidden(false)
			popAll()
		}
	}

	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {
			onRestartCheck: d.restartCheck()
			onStartAuth: root.startAuth()
			onCancelClicked: d.cancel()
		}
	}

	Component {
		id: checkIDCardWorkflow

		CheckIDCardWorkflow {
			onRestartCheck: d.restartCheck()
			onStartAuth: root.startAuth()
			onCancel: d.cancel()
		}
	}

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

			source: "qrc:///images/mobile/device.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent
		}

		GPane {
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("To use the online identification function, your device must meet certain technical requirements. Furthermore, the online identification function must be activated.")
			}

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("Check if your smartphone and ID card are ready for use.")
			}
		}

		GSpacer { Layout.fillHeight: true }

		GButton {
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Start check")
			icon.source: "qrc:///images/mobile/device.svg"

			onClicked: {
				if (ApplicationModel.nfcState === ApplicationModel.NFC_UNAVAILABLE) {
					setLockedAndHidden()
					push(checkIDCardResultView, { result: CheckIDCardModel.NO_NFC })
				} else {
					d.startCheck()
				}
			}
		}
	}
}
