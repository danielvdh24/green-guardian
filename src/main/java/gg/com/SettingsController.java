package gg.com;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.RadioButton;
import javafx.scene.control.ToggleGroup;

import java.io.IOException;

public class SettingsController {
    @FXML
    public ToggleGroup pageSelect, frequencySelect;
    @FXML
    private ChoiceBox<String> notificationSelect;
    @FXML
    private final String[] notifications = {"Off", "On"};
    @FXML
    private RadioButton spreadSheetSelect, graphSelect, commandsSelect, settingsSelect;
    @FXML
    private RadioButton fiveSecondSelect, oneMinuteSelect, fiveMinuteSelect, thirtyMinuteSelect;
    private static double graphDelay;

    @FXML
    public void initialize() {
        notificationSelect.getItems().addAll(notifications);
        notificationSelect.setOnAction(this::getNotification);
        initValues();
    }

    @FXML
    private void getNotification(ActionEvent event) {
        Preferences preferences = Preferences.getPreferences();
        preferences.setNotifications(!notificationSelect.getValue().equals("Off"));
        Preferences.writePreferenceToFile(preferences);
    }

    @FXML
    private void onSetDefaultPage() {
        String scene = null;
        if (spreadSheetSelect.isSelected()) {
            scene = "SpreadsheetScene";
        }
        if (graphSelect.isSelected()) {
            scene = "GraphScene";
        }
        if (commandsSelect.isSelected()) {
            scene = "CommandsScene";
        }
        if (settingsSelect.isSelected()) {
            scene = "SettingsScene";
        }
        Preferences preferences = Preferences.getPreferences();
        preferences.setScene(scene);
        Preferences.writePreferenceToFile(preferences);
    }

    @FXML
    private void onSetGraphFrequency() {
        int delay = 0;
        if (fiveSecondSelect.isSelected()) {
            delay = 5;
        }
        if (oneMinuteSelect.isSelected()) {
            delay = 60;
        }
        if (fiveMinuteSelect.isSelected()) {
            delay = 300;
        }
        if (thirtyMinuteSelect.isSelected()) {
            delay = 1800;
        }
        graphDelay = delay;
        Preferences preferences = Preferences.getPreferences();
        preferences.setInterval(delay);
        Preferences.writePreferenceToFile(preferences);
    }

    private void initValues() {
        Preferences preferences = Preferences.getPreferences();
        if (preferences.isNotifications()) {
            notificationSelect.setValue(notifications[1]);
        } else {
            notificationSelect.setValue(notifications[0]);
        }

        int interval = preferences.getInterval();

        switch (interval) {
            case 5:
                fiveSecondSelect.setSelected(true);
                break;
            case 60:
                oneMinuteSelect.setSelected(true);
                break;
            case 300:
                fiveMinuteSelect.setSelected(true);
                break;
            case 1800:
                thirtyMinuteSelect.setSelected(true);
                break;
            default:
                fiveSecondSelect.setSelected(true);
                preferences.setInterval(5);
                break;
        }

        String scene = preferences.getScene();

        switch (scene) {
            case "SpreadSheetScene":
                spreadSheetSelect.setSelected(true);
                break;
            case "GraphScene":
                graphSelect.setSelected(true);
                break;
            case "CommandsScene":
                commandsSelect.setSelected(true);
                break;
            case "SettingsScene":
                settingsSelect.setSelected(true);
                break;
            default:
                spreadSheetSelect.setSelected(true);
                preferences.setScene("SpreadsheetScene");
                break;
        }
    }

    public static void loadDelay() {
        Preferences preferences = Preferences.getPreferences();
        graphDelay = preferences.getInterval();
        if (!(graphDelay == 5 || graphDelay == 60 || graphDelay == 300 || graphDelay == 1800)) {
            graphDelay = 5;
        }
    }

    public static double getDelay() {
        return graphDelay;
    }

    public void onSpreadSheetButtonClick() throws IOException {
        App.setRoot("SpreadsheetScene");
    }

    public void onCommandsButtonClick() throws IOException {
        App.setRoot("CommandsScene");
    }

    public void onGraphButtonClick() throws IOException {
        App.setRoot("GraphScene");
    }
}