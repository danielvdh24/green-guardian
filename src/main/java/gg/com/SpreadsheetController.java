package gg.com;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;

import java.io.IOException;

public class SpreadsheetController 
{
    @FXML
    private Label PlantID;
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

    public void initialize() 
    {
        //TODO: Read file and put values here
        PlantID.setText(null);
        PlantName.setText(null);
        TempC.setText(null);
        HumC.setText(null);
        LightC.setText(null);
        Status.setText(null);

        LightO.setText(null);
        TempO.setText(null);
        HumO.setText(null);
    }

    public void update()
    {
        //TODO: Get MQTT values here
    }

    public void onChange()
    {
        //TODO: Trigger to save to file on user input
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
