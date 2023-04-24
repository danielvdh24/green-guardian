package gg.com;

import javafx.fxml.FXML;
import javafx.scene.control.TableView;
import javafx.scene.control.TableColumn;
import javafx.collections.ObservableList;
import javafx.collections.FXCollections;
import javafx.scene.control.TableColumn.CellDataFeatures;
import javafx.beans.value.*;
import javafx.beans.property.*;
import javafx.util.*;

public class SpreadsheetController 
{

    @FXML
    
    TableView<TableEntry> Table;
    TableColumn<TableEntry,Integer> PlantID;
    TableColumn<TableEntry,String> Name;
    TableColumn<TableEntry,Integer> Current_Temperature;
    TableColumn<TableEntry,Integer> Current_Humidity;
    TableColumn<TableEntry,Integer> Current_Light;
    TableColumn<TableEntry,Integer> Optimal_Temperature;
    TableColumn<TableEntry,Integer> Optimal_Humidity;
    TableColumn<TableEntry,Integer> Optimal_Light;
    TableColumn<TableEntry,String> Status;
    public void initialize() {
        ObservableList<TableEntry> data = FXCollections.observableArrayList();
        data.add(new TableEntry(0, "1", 0, 0, 0, 0, 0, 0, "1"));
        Table = new TableView<>(data);
        PlantID = new TableColumn<>("Plant ID");
        Name = new TableColumn<TableEntry,String>("Name");
        Current_Temperature = new TableColumn<>("Temperature");
        Current_Humidity = new TableColumn<>("Humidity");
        Current_Light = new TableColumn<>("Light");
        Optimal_Temperature = new TableColumn<>("Temperature");
        Optimal_Humidity = new TableColumn<>("Humidity");
        Optimal_Light = new TableColumn<>("Light");
        Status = new TableColumn<>("Status");

        /// TO BE IMPLEMENTED 

        /*PlantID.setCellValueFactory(new Callback<CellDataFeatures<TableEntry, Integer>, ObservableValue<Integer>>() {
            public ObservableValue<Integer> call(CellDataFeatures<TableEntry, Integer> p) 
            {
                // p.getValue() returns the Person instance for a particular TableView row
                return p.getValue().IDProperty();
            }
        })*/
    }
}
