/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Type.ApplicationModel 1.0

Item {
	function open() {
		ApplicationModel.showAppStoreRatingDialog();
	}
}