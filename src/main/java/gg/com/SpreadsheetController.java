package gg.com;

import javafx.fxml.FXML;
import javafx.scene.control.TableView;
import javafx.scene.control.TableColumn;
import javafx.collections.ObservableList;
import javafx.collections.FXCollections;
import javafx.scene.control.TableColumn.CellDataFeatures;
import javafx.beans.value.*;
import javafx.util.*;
import javafx.scene.control.Button;

import java.io.IOException;

public class SpreadsheetController 
{

    @FXML
    ObservableList<TableEntry> data = FXCollections.observableArrayList();
    @FXML
    private Button graphButton, commandsButton, settingsButton;
    @FXML
    public TableView<TableEntry> Table;
    @FXML
    public TableColumn<TableEntry,String> PlantID;
    @FXML
    public TableColumn<TableEntry,String> Name;
    @FXML
    public TableColumn<TableEntry,String> Current_Temperature;
    @FXML
    public TableColumn<TableEntry,String> Current_Humidity;
    @FXML
    public TableColumn<TableEntry,String> Current_Light;
    @FXML
    public TableColumn<TableEntry,String> Optimal_Temperature;
    @FXML
    public TableColumn<TableEntry,String> Optimal_Humidity;
    @FXML
    public TableColumn<TableEntry,String> Optimal_Light;
    @FXML
    public TableColumn<TableEntry,String> Status;

    public void initialize() 
    {
        data.add(new TableEntry(0, "1", 0, 0, 0, 0, 0, 0, "1"));
        Table.setItems(data);

        PlantID.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().IDProperty();}
                }
            );
        
        Name.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().nameProperty();}
                }
            );

        Current_Temperature.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().CTProperty();}
                }
            );
        
        Current_Humidity.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().CHProperty();}
                }
            );

        Current_Light.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().CLProperty();}
                }
            );
        
        Optimal_Temperature.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().OTProperty();}
                }
            );

        Optimal_Humidity.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().OHProperty();}
                }
            );
        
        Optimal_Light.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().OLProperty();}
                }
            );
        
        Status.setCellValueFactory
            (
            new Callback<CellDataFeatures<TableEntry, String>, ObservableValue<String>>() 
                {
                    public ObservableValue<String> call(CellDataFeatures<TableEntry, String> p) {return p.getValue().statusProperty();}
                }
            );
        
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
