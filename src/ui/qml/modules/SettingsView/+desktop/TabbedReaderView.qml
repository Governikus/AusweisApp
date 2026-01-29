/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models

import Governikus.Global
import Governikus.MultiInfoView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

SectionPage {
	id: root

	//: DESKTOP
	title: qsTr("Card readers")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Action.Back

		onNavigationActionClicked: root.leaveView()
	}

	Keys.onPressed: event => {
		tabbedPane.handleKeyPress(event);
	}

	ReaderDetection {
		onNewPcscReaderDetected: {
			if (ApplicationModel.screenReaderRunning) {
				root.push(readerFoundConfirmation, {
					type: ReaderFoundConfirmation.ReaderType.PCSC
				});
			} else {
				root.leaveView();
			}
		}
		onNewRemoteReaderDetected: {
			if (ApplicationModel.screenReaderRunning) {
				root.push(readerFoundConfirmation, {
					type: ReaderFoundConfirmation.ReaderType.REMOTE
				});
			} else {
				root.leaveView();
			}
		}
	}
	Component {
		id: readerFoundConfirmation

		ReaderFoundConfirmation {
			title: root.title

			onLeaveView: {
				root.pop();
				root.leaveView();
			}
		}
	}
	TabbedPane {
		id: tabbedPane

		sectionsModel: [qsTr("Smartphone as card reader"), qsTr("USB card reader")]

		contentObjectModel: ObjectModel {
			Component {
				RemoteReaderView {
					onPairDevice: pDeviceId => {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							root.push(connectSacView);
						}
					}
					onShowNoSacFoundInfo: root.push(noSacFoundInfo)

					Component {
						id: noSacFoundInfo

						MultiInfoView {
							progress: root.progress

							infoContent: MultiInfoData {
								contentType: MultiInfoData.Type.NO_SAC_FOUND
							}
							titleBarSettings: TitleBarSettings {
								navigationAction: NavigationAction.Action.Back

								onNavigationActionClicked: root.pop()
							}
						}
					}
					Component {
						id: connectSacView

						ConnectSacView {
							onPairingFailed: pop()
							onPairingSuccessful: {
								pop();
								root.leaveView();
							}
						}
					}
				}
			}
			Component {
				CardReaderView {
				}
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Style.dimens.pane_padding
			right: parent.right
			top: parent.top
		}
	}
}
