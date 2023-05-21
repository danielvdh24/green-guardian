package gg.com;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.scene.control.TextFormatter;

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
    private static String timeStart, timeEnd;
    private static Boolean online, manual, light;
    private static App publisher;

    public void initialize() {
        setTwoDigitNumberFilter(startHour);
        setTwoDigitNumberFilter(endHour);
        setTwoDigitNumberFilter(startMinute);
        setTwoDigitNumberFilter(endMinute);

        Preferences preferences = Preferences.getPreferences();
        String startTime = preferences.getStartTime();
        String startTimeHour = startTime.substring(0,2);
        String startTimeMin = startTime.substring(2);
        String endTime = preferences.getEndTime();
        String endTimeHour = endTime.substring(0,2);
        String endTimeMin = endTime.substring(2);
        startHour.setText(startTimeHour);
        startMinute.setText(startTimeMin);
        endHour.setText(endTimeHour);
        endMinute.setText(endTimeMin);
        switchState(preferences.getOnline());
        online = preferences.getOnline();
        switchManual(preferences.getManual());
        manual = preferences.getManual();
        switchLight(preferences.getLight());
        light = preferences.getLight();
    }

    private void setTwoDigitNumberFilter(TextField textField) {
        TextFormatter<String> textFormatter = new TextFormatter<>(change -> {
            String newText = change.getControlNewText();
            if (newText.matches("\\d{0,2}")) {
                return change;
            }
            return null;
        });
        textField.setTextFormatter(textFormatter);
    }

    @FXML
    private void handlePowerButtonClicked() {
        online = !online;
        switchState(online);
        Preferences preferences = Preferences.getPreferences();
        preferences.setOnline(online);
        Preferences.writePreferenceToFile(preferences);
        publisher.publish(getOnline());
    }

    @FXML
    private void handleManualButtonClicked(){
        manual = !manual;
        switchManual(manual);
        Preferences preferences = Preferences.getPreferences();
        preferences.setManual(manual);
        Preferences.writePreferenceToFile(preferences);
        publisher.publish(getManual());
    }

    @FXML
    private void handleLightButtonClicked(){
        light = !light;
        switchLight(light);
        Preferences preferences = Preferences.getPreferences();
        preferences.setLight(light);
        Preferences.writePreferenceToFile(preferences);
        publisher.publish(getLight());
    }

    @FXML
    private void handleSaveButtonClicked(){
        saveErrorLabel.setVisible(false);
        if(startHour.getText().equals("") || !(startHour.getText().matches("\\d{2}")) ||
                startMinute.getText().equals("") || !(startMinute.getText().matches("\\d{2}")) ||
                endHour.getText().equals("") || !(endHour.getText().matches("\\d{2}")) ||
                endMinute.getText().equals("") || !(endMinute.getText().matches("\\d{2}"))){
            saveErrorLabel.setVisible(true);
            return;
        }

        int startHourInt = Integer.parseInt(startHour.getText());
        int startMinuteInt = Integer.parseInt(startMinute.getText());
        int endHourInt = Integer.parseInt(endHour.getText());
        int endMinuteInt = Integer.parseInt(endMinute.getText());

        if (startHourInt > 24 ||
                startMinuteInt > 59 ||
                endHourInt > 24 ||
                endMinuteInt > 59){
            saveErrorLabel.setVisible(true);
            return;
        }

        int startTime = startHourInt * 60 + startMinuteInt;
        int endTime = endHourInt * 60 + endMinuteInt;

        if (startTime >= endTime - 1){
            saveErrorLabel.setVisible(true);
            return;
        }

        timeStart = startHour.getText() + startMinute.getText();
        timeEnd = endHour.getText() + endMinute.getText();
        Preferences preferences = Preferences.getPreferences();
        preferences.setStartTime(timeStart);
        preferences.setEndTime(timeEnd);
        Preferences.writePreferenceToFile(preferences);
        publisher.publish(getLight());
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
        if (light) {
            lightButtonCircle.setStroke(Color.YELLOW);
            lightButtonLabel.setText("Keep Lights ON");
        } else {
            lightButtonCircle.setStroke(Color.LIGHTGREY);
            lightButtonLabel.setText("Keep Lights OFF");
        }
    }

    private static String getManual(){
        if(manual){
            return "automaticOff;";
        } else {
            return "automaticOn;";
        }
    }

    private static String getOnline(){
        if(online){
            return "activeOn;";
        } else {
            return "activeOff;";
        }
    }
        
    private static String getLight(){
        if (light) {
            return "timeScedOn" + ";" + timeStart + ";" + timeEnd + ";";
        } else {
            return "timeScedOff" + ";" + timeStart + ";" + timeEnd + ";";
        }
    }

    public static void setPublisher(App app){
        publisher = app;
        Preferences preferences = Preferences.getPreferences();
        timeStart = preferences.getStartTime();
        timeEnd = preferences.getEndTime();
        online = preferences.getOnline();
        manual = preferences.getManual();
        light = preferences.getLight();
        publisher.publish(getOnline());
        publisher.publish(getManual());
        publisher.publish(getLight());
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
