package gg.com;

import java.io.IOException;

public class CommandsController {

    public void onSpreadSheetButtonClick() throws IOException {
        App.setRoot("SpreadsheetScene");
    }

    public void onSettingsButtonClick() throws IOException {
        App.setRoot("SettingsScene");
    }

    public void onGraphButtonClick() throws IOException {
        App.setRoot("GraphScene");
    }
}
