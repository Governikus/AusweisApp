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
	signal startAuth

	sectionPageFlickable: contentItem

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: show(UiModule.DEFAULT)
	}

	onStartAuth: {
		popAll();
		show(UiModule.SELF_AUTHENTICATION);
	}

	QtObject {
		id: d
		function cancel() {
			setLockedAndHidden(false);
			popAll();
		}
		function restartCheck() {
			popAll();
			startCheck();
		}
		function startCheck() {
			setLockedAndHidden();
			push(checkIDCardWorkflow);
		}
	}
	Component {
		id: checkIDCardResultView
		CheckIDCardResultView {
			onCancelClicked: d.cancel()
			onRestartCheck: d.restartCheck()
			onStartAuth: root.startAuth()
		}
	}
	Component {
		id: checkIDCardWorkflow
		CheckIDCardWorkflow {
			onCancel: d.cancel()
			onRestartCheck: d.restartCheck()
			onStartAuth: root.startAuth()
		}
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
			source: "qrc:///images/mobile/device.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent
		}
		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true

			GText {

				//: LABEL ANDROID IOS
				text: qsTr("To use the online identification function, your device must meet certain technical requirements. Furthermore, the online identification function must be activated.")
				width: parent.width
			}
			GText {

				//: LABEL ANDROID IOS
				text: qsTr("Check if your smartphone and ID card are ready for use.")
				width: parent.width
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			icon.source: "qrc:///images/mobile/device.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Start check")

			onClicked: {
				if (ApplicationModel.nfcState === ApplicationModel.NFC_UNAVAILABLE) {
					setLockedAndHidden();
					push(checkIDCardResultView, {
							"result": CheckIDCardModel.NO_NFC
						});
				} else {
					d.startCheck();
				}
			}
		}
	}
}
