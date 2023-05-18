package gg.com;

import java.io.FileWriter;
import java.io.IOException;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.layout.Pane;

public class SpreadsheetController 
{
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

    public void initialize() 
    {
        Reader reader = new Reader("SpreadsheetData.txt");
        setTwoDigitNumberFilter(TempO);
        setTwoDigitNumberFilter(HumO);
        setTwoDigitNumberFilter(LightO);

        PlantName.setText(reader.nextLine());

        update();
        
        LightO.setText(reader.nextLine());
        TempO.setText(reader.nextLine());
        HumO.setText(reader.nextLine());

        if(Integer.parseInt(TempC.getText())>Integer.parseInt(TempO.getText())
        && Integer.parseInt(LightC.getText())>Integer.parseInt(LightO.getText())
        && Integer.parseInt( HumC.getText())>Integer.parseInt( HumO.getText()))Status.setText("OK");
        else Status.setText("Alert");

    }

    public void update()
    {
        //TODO: Get MQTT values here
        LightC.setText("100");
        TempC.setText("100");
        HumC.setText("100");

    }

    private void setTwoDigitNumberFilter(TextField textField) {
        TextFormatter<String> textFormatter = new TextFormatter<>(change -> {
            String newText = change.getControlNewText();

            try
            {   
                if(newText.length()>1&&newText.charAt(0)=='0')
                {
                    return null;
                }
                if(newText.length()==0)
                {
                    textField.setText("0");
                    return null;
                }
                else if(Integer.parseInt(newText)>100||newText.length()>3)
                {
                    textField.setText("100");
                    return null;
                }
                return change;
            }
            catch(Exception e)
            {
                return null;
            }
            
        });
        textField.setTextFormatter(textFormatter);
    }

    public void onChange()
    {
        if(Integer.parseInt(TempC.getText())>Integer.parseInt(TempO.getText())
        && Integer.parseInt(LightC.getText())>Integer.parseInt(LightO.getText())
        && Integer.parseInt( HumC.getText())>Integer.parseInt( HumO.getText()))Status.setText("OK");
        else Status.setText("Alert");
        String NewData = PlantName.getText()+"\n"+LightO.getText()+"\n"+TempO.getText()+"\n"+HumO.getText()+"\n";
        DocWriter.write("SpreadsheetData.txt",NewData);   
    }

    public void ChangeFocus()
    {
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
