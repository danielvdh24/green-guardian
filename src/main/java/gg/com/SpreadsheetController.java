package gg.com;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;

public class SpreadsheetController {
    @FXML
    private Pane MainPane;
    @FXML
    private TextField PlantName;
    @FXML
    private Label HumC;
    @FXML
    private Label LightC;
    @FXML
    private Label TempC;
    @FXML
    private TextField HumO;
    @FXML
    private TextField LightO;
    @FXML
    private TextField TempO;
    @FXML
    private Label Status;

    public void initialize() {
        Path dataFilePath = Paths.get("config", "SpreadsheetData.txt");
        try {
            if (!Files.exists(dataFilePath)) {
                Files.createFile(dataFilePath);
                String NewData = "Plant 1" + "\n" + "50" + "\n" + "50" + "\n" + "50" + "\n";
                DocWriter.write("SpreadsheetData.txt", NewData);
            }
        } catch (Exception e) {}

        Reader reader = new Reader("SpreadsheetData.txt");
        setTwoDigitNumberFilter(TempO);
        setTwoDigitNumberFilter(HumO);
        setTwoDigitNumberFilter(LightO);

        PlantName.setText(reader.nextLine());

        update();

        LightO.setText(reader.nextLine());
        TempO.setText(reader.nextLine());
        HumO.setText(reader.nextLine());

        CheckStatus();
    }

    public void update() {
        Reader reader = new Reader("GraphData.txt");
        for (int i = 0; i < 1008 - 3; i++) reader.nextInt();
        LightC.setText(String.valueOf(reader.nextInt()));
        TempC.setText(String.valueOf(reader.nextInt()));
        HumC.setText(String.valueOf(reader.nextInt()));
    }

    private void setTwoDigitNumberFilter(TextField textField) {
        TextFormatter<String> textFormatter = new TextFormatter<>(change -> {
            String newText = change.getControlNewText();

            try {
                if (newText.length() > 1 && newText.charAt(0) == '0') {
                    return null;
                }
                if (newText.length() == 0) {
                    textField.setText("0");
                    return null;
                } else if (Integer.parseInt(newText) > 100 || newText.length() > 3) {
                    textField.setText("100");
                    return null;
                }
                return change;
            } catch (Exception e) {
                return null;
            }

        });
        textField.setTextFormatter(textFormatter);
    }

    public void CheckStatus() {
        if (Integer.parseInt(TempC.getText()) > Integer.parseInt(TempO.getText())
                && Integer.parseInt(LightC.getText()) > Integer.parseInt(LightO.getText())
                && Integer.parseInt(HumC.getText()) > Integer.parseInt(HumO.getText())) {
            Status.setText("OK");
            Status.setTextFill(Color.GREEN);
        } else {
            Status.setText("Alert");
            Status.setTextFill(Color.RED);
        }
    }

    public void onChange() {
        CheckStatus();
        String NewData = PlantName.getText() + "\n" + LightO.getText() + "\n" + TempO.getText() + "\n" + HumO.getText() + "\n";
        DocWriter.write("SpreadsheetData.txt", NewData);
    }

    public void ChangeFocus() {
        MainPane.requestFocus();
    }

    public void onGraphButtonClick() throws IOException {
        App.setRoot("GraphScene");
    }

    public void onCommandsButtonClick() throws IOException {
        App.setRoot("CommandsScene");
    }

    public void onSettingsButtonClick() throws IOException {
        App.setRoot("SettingsScene");
    }
}