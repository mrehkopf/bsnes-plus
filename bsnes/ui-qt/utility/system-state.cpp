void Utility::modifySystemState(system_state_t systemState) {
  fileBrowser->close();  //avoid edge case oddities (eg movie playback window still open from previous game)
  state.resetHistory();  //do not allow rewinding past a destructive system action
  movie.stop();  //movies cannot continue to record after destructive system actions

  video.clear();
  audio.clear();

  switch(systemState) {
    case LoadCartridge: {
      //must call cartridge.load_cart_...() before calling modifySystemState(LoadCartridge)
      if(SNES::cartridge.loaded() == false) break;
      cartridge.loadCheats();

      application.power = true;
      application.pause = false;
      SNES::system.power();

      //warn if unsupported hardware detected
      string chip;
      if(SNES::cartridge.has_st0018()) chip = "ST-0018";
      if(chip != "") {
        QMessageBox::warning(mainWindow, "Warning", string()
        << "<p><b>Warning:</b><br> The " << chip << " chip was detected, which is not fully emulated yet.<br>"
        << "It is unlikely that this title will work properly.</p>");
      }

      showMessage(string()
      << "Loaded " << cartridge.name
      << (cartridge.patchApplied ? ", and applied UPS patch." : "."));
      mainWindow->setWindowTitle(string() << cartridge.name << " - " << SNES::Info::Name << " v" << SNES::Info::Version);
      #if defined(DEBUGGER)
      debugger->echo(string() << "Loaded " << cartridge.name << ".<br>");
      #endif
    } break;

    case UnloadCartridge: {
      if(SNES::cartridge.loaded() == false) break;  //no cart to unload?
      cartridge.saveCheats();

      SNES::system.unload();     //flush all memory to memory::* devices
      cartridge.saveMemory();    //save memory to disk
      SNES::cartridge.unload();  //deallocate memory

      application.power = false;
      application.pause = true;

      showMessage(string() << "Unloaded " << cartridge.name << ".");
      mainWindow->setWindowTitle(string() << SNES::Info::Name << " v" << SNES::Info::Version);
    } break;

    case PowerOn: {
      if(SNES::cartridge.loaded() == false || application.power == true) break;

      application.power = true;
      application.pause = false;
      SNES::system.power();

      showMessage("Power on.");
    } break;

    case PowerOff: {
      if(SNES::cartridge.loaded() == false || application.power == false) break;

      application.power = false;
      application.pause = true;

      showMessage("Power off.");
    } break;

    case PowerCycle: {
      if(SNES::cartridge.loaded() == false) break;

      application.power = true;
      application.pause = false;
      SNES::system.power();

      showMessage("System power was cycled.");
    } break;

	case ReloadCartridge: {
		 if(application.currentRom != "")
			 cartridge.loadNormal(application.currentRom);
		 showMessage(string() << "Reloaded " << cartridge.name << ".");
	 } break;

    case Reset: {
      if(SNES::cartridge.loaded() == false || application.power == false) break;

      application.pause = false;
      SNES::system.reset();

      showMessage("System was reset.");
    } break;
  }

  mainWindow->syncUi();
  #if defined(DEBUGGER)
  debugger->modifySystemState(systemState);
  debugger->synchronize();
  #endif
  cheatEditorWindow->synchronize();
  cheatFinderWindow->synchronize();
  stateManagerWindow->reload();
}
