/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Type

Item {
	id: root

	signal newPcscReaderDetected
	signal newRemoteReaderDetected

	Component.onCompleted: d.updateLastReaderCount()

	QtObject {
		id: d

		property int lastPcscCount: 0
		property int lastRemoteCount: 0

		function updateLastReaderCount() {
			d.lastRemoteCount = ApplicationModel.availableRemoteReader;
			d.lastPcscCount = ApplicationModel.availablePcscReader;
		}
	}
	Connections {
		function onFireAvailableReaderChanged() {
			if (root.visible && root.enabled) {
				if (ApplicationModel.availableRemoteReader > d.lastRemoteCount) {
					root.newRemoteReaderDetected();
				}
				if (ApplicationModel.availablePcscReader > d.lastPcscCount) {
					root.newPcscReaderDetected();
				}
			}
			d.updateLastReaderCount();
		}

		target: ApplicationModel
	}
}
