/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.ChangePinView
import Governikus.TitleBar
import Governikus.MultiInfoView

BaseTransportPinReminderView {
	id: root

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Cancel

		onNavigationActionClicked: root.cancel()
	}

	onShowInfoView: pContentType => push(multiInfoView, {
			infoContent: infoData.createObject(root, {
				contentType: pContentType
			})
		})
	onShowTransportPinInfo: push(changeTransportInfoView)

	Component {
		id: infoData

		MultiInfoData {
		}
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back

				onNavigationActionClicked: root.pop()
			}
		}
	}
	Component {
		id: changeTransportInfoView

		ChangeTransportPinInfoView {
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: {
				root.pop();
				root.transportPinKnown();
			}
		}
	}
}
