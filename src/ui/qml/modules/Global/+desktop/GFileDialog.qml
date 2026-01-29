/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtCore
import QtQuick
import QtQuick.Dialogs

FileDialog {
	function selectFile(pDefaultFilename) {
		let isFileUrl = /^file:\/\//;
		let folder = isFileUrl.test(currentFolder) ? currentFolder : "file://" + currentFolder;
		selectedFile = folder + "/" + pDefaultFilename;
		open();
	}

	currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
	fileMode: FileDialog.SaveFile

	Component.onDestruction: reject()
}
