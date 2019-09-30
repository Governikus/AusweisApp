function Controller()
{
	installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
	installer.setMessageBoxAutomaticAnswer("TargetDirectoryInUse", QMessageBox.Ok);
	installer.setMessageBoxAutomaticAnswer("cancelInstallation", QMessageBox.Yes);
}

Controller.prototype.WelcomePageCallback = function()
{
	console.log("Welcome");
	var widget = gui.currentPageWidget();
	gui.clickButton(buttons.NextButton);
	widget.completeChanged.connect(function()
	{
		gui.clickButton(buttons.NextButton);
	});
}

Controller.prototype.CredentialsPageCallback = function()
{
	console.log("Credentials");
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function()
{
	console.log("Introduction");
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
	console.log("TargetDirectory: " + installer.value("TargetDir"));
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function()
{
	var packages = installer.value("Packages")
	console.log("ComponentSelection: " + packages);
	var widget = gui.currentPageWidget();
	widget.deselectAll();

	packages = packages.split(",");
	var components = installer.components();
	for (var i in packages)
	{
		pkg = packages[i];
		for (var j in components)
		{
			if (components[j].name === pkg)
			{
				widget.selectComponent(pkg);
				break;
			}
		}
	}

	gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function()
{
	console.log("LicenseAgreement");
	var widget = gui.currentPageWidget();
	widget.AcceptLicenseRadioButton.setChecked(true);
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{
	console.log("ReadyForInstallation");
	gui.clickButton(buttons.CommitButton);
}

Controller.prototype.PerformInstallationPageCallback = function()
{
	console.log("PerformInstallation");
	installer.installationFinished.connect(function()
	{
		gui.clickButton(buttons.NextButton);
	});
}

Controller.prototype.FinishedPageCallback = function()
{
	console.log("Finished");
	var widget = gui.currentPageWidget();
	if (widget.LaunchQtCreatorCheckBoxForm)
	{
		widget.LaunchQtCreatorCheckBoxForm.launchQtCreatorCheckBox.setChecked(false);
	}
	else if (widget.RunItCheckBox)
	{
		widget.RunItCheckBox.setChecked(false);
	}
	gui.clickButton(buttons.FinishButton);
}
