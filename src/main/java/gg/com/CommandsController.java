package gg.com;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.scene.control.TextFormatter;
import javafx.util.converter.IntegerStringConverter;

import java.io.IOException;

public class CommandsController {

    @FXML
    private Circle powerButtonCircle, manualButtonCircle, lightButtonCircle;
    @FXML
    private Rectangle coverAutoMode;
    @FXML
    private Label powerButtonLabel, manualButtonLabel, lightButtonLabel, saveErrorLabel;
    @FXML
    private TextField startHour, endHour, startMinute, endMinute;
    private Boolean online, manual, light;

    public void initialize() {
        setTwoDigitNumberFilter(startHour);
        setTwoDigitNumberFilter(endHour);
        setTwoDigitNumberFilter(startMinute);
        setTwoDigitNumberFilter(endMinute);

        Preferences preferences = Preferences.getPreferences();
        switchState(preferences.getOnline());
        online = preferences.getOnline();
        switchManual(preferences.getManual());
        manual = preferences.getManual();
        switchLight(preferences.getLight());
        light = preferences.getLight();
    }

    private void setTwoDigitNumberFilter(TextField textField) {
        TextFormatter<Integer> textFormatter = new TextFormatter<>(
                new IntegerStringConverter(),
                null,
                c -> {
                    if (c.getControlNewText().matches("\\d{0,2}")) {
                        return c;
                    }
                    return null;
                }
        );
        textField.setTextFormatter(textFormatter);
    }

    @FXML
    private void handlePowerButtonClicked() {
        online = !online;
        switchState(online);
        Preferences preferences = Preferences.getPreferences();
        preferences.setOnline(online);
        Preferences.writePreferenceToFile(preferences);
        // mqtt switch command
    }

    @FXML
    private void handleManualButtonClicked(){
        manual = !manual;
        switchManual(manual);
        Preferences preferences = Preferences.getPreferences();
        preferences.setManual(manual);
        Preferences.writePreferenceToFile(preferences);
        // mqtt manual mode command
    }

    @FXML
    private void handleLightButtonClicked(){
        light = !light;
        switchLight(light);
        Preferences preferences = Preferences.getPreferences();
        preferences.setLight(light);
        Preferences.writePreferenceToFile(preferences);
        // mqtt light mode command
    }

    @FXML
    private void handleSaveButtonClicked(){
        saveErrorLabel.setVisible(false);
        if(startHour.getText().equals("") || startMinute.getText().equals("") || endHour.getText().equals("") || endMinute.getText().equals("")){
            saveErrorLabel.setVisible(true);
            return;
        } else if (Integer.parseInt(startHour.getText()) > 24 || Integer.parseInt(startMinute.getText()) > 60 || Integer.parseInt(endHour.getText()) > 24 || Integer.parseInt(endMinute.getText()) > 60){
            saveErrorLabel.setVisible(true);
            return;
        }
        String formattedStartHour = String.format("%02d", Integer.parseInt(startHour.getText()));
        String formattedStartMinute = String.format("%02d", Integer.parseInt(startMinute.getText()));
        String formattedEndHour = String.format("%02d", Integer.parseInt(endHour.getText()));
        String formattedEndMinute = String.format("%02d", Integer.parseInt(endMinute.getText()));
        String timeStart = formattedStartHour + ":" + formattedStartMinute;
        String timeEnd = formattedEndHour + ":" + formattedEndMinute;
        System.out.println(timeStart);
        System.out.println(timeEnd);
    }

    private void switchState(boolean online){
        if(online){
            powerButtonCircle.setStroke(Color.LIME);
            powerButtonLabel.setText("Turn OFF");
        } else {
            powerButtonCircle.setStroke(Color.RED);
            powerButtonLabel.setText("Turn ON");
        }
    }

    private void switchManual(boolean manual){
        if(manual){
            manualButtonCircle.setStroke(Color.SANDYBROWN);
            manualButtonLabel.setText("Now in MANUAL Mode");
            coverAutoMode.setVisible(false);
        } else {
            manualButtonCircle.setStroke(Color.DEEPSKYBLUE);
            manualButtonLabel.setText(("Now in AUTOMATIC Mode"));
            coverAutoMode.setVisible(true);
            startHour.getParent().requestFocus();
            startMinute.getParent().requestFocus();
            endHour.getParent().requestFocus();
            endMinute.getParent().requestFocus();
        }
    }

    private void switchLight(boolean light){
        if(light){
            lightButtonCircle.setStroke(Color.YELLOW);
            lightButtonLabel.setText("Keep Lights ON");
        } else {
            lightButtonCircle.setStroke(Color.LIGHTGREY);
            lightButtonLabel.setText("Keep Lights OFF");
        }
    }

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
