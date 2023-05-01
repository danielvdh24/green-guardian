package gg.com;

import javafx.fxml.FXML;
import javafx.scene.control.Button;

import java.io.IOException;

public class CommandsController {
    @FXML
    private Button graphButton, settingsButton, spreadSheetButton;

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
