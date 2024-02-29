/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQml.Models
import Governikus.Global
import Governikus.ResultView
import Governikus.TitleBar
import Governikus.Type.ApplicationModel
import Governikus.Type.NumberModel
import Governikus.Type.RemoteServiceModel
import Governikus.View

SectionPage {
	id: root

	enum SubView {
		None,
		ConnectSacView,
		PcscReaderFoundView
	}

	readonly property int availableReader: ApplicationModel.availablePcscReader
	readonly property int currentView: d.view
	property int lastReaderCount: 0
	property alias paneAnchors: tabbedPane.anchors
	property alias rootEnabled: titleBarAction.rootEnabled

	signal closeView

	titleBarAction: TitleBarAction {
		id: titleBarAction

		rootEnabled: false

		//: LABEL DESKTOP
		text: qsTr("Card Readers")

		customSubAction: NavigationAction {
			type: NavigationAction.Action.Back

			onClicked: closeView()
		}

		onClicked: d.view = TabbedReaderView.SubView.None
	}

	Component.onCompleted: lastReaderCount = availableReader
	onAvailableReaderChanged: {
		if (visible && availableReader > lastReaderCount) {
			d.view = TabbedReaderView.SubView.PcscReaderFoundView;
			updateTitleBarActions();
		}
		lastReaderCount = availableReader;
	}
	onVisibleChanged: d.view = TabbedReaderView.SubView.None

	QtObject {
		id: d

		property int precedingView
		property int view
	}
	ResultView {
		icon: "qrc:///images/desktop/workflow_idcard_usb.svg"
		text: qsTr("Found new USB card reader that is suitable for the ID card. The workflow may now be continued.")
		visible: d.view === TabbedReaderView.SubView.PcscReaderFoundView

		onNextView: root.closeView()
	}
	TabbedPane {
		id: tabbedPane

		sectionsModel: [qsTr("Smartphone as card reader"), qsTr("USB card reader")]
		visible: d.view === TabbedReaderView.SubView.None

		contentObjectModel: ObjectModel {
			Component {
				RemoteReaderView {
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width

					onPairDevice: pDeviceId => {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							d.view = TabbedReaderView.SubView.ConnectSacView;
							updateTitleBarActions();
						}
					}
					onUnpairDevice: pDeviceId => RemoteServiceModel.forgetDevice(pDeviceId)
				}
			}
			Component {
				CardReaderView {
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width
				}
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Constants.pane_padding
			right: parent.right
			top: parent.top
		}
	}
	ConnectSacView {
		id: connectSacView

		rootEnabled: root.rootEnabled
		visible: d.view === TabbedReaderView.SubView.ConnectSacView

		onPairingFailed: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
		onPairingSuccessful: root.closeView()
	}
}
