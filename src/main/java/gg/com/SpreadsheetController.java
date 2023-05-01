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

public class SpreadsheetController 
{

    @FXML
    ObservableList<TableEntry> data = FXCollections.observableArrayList();
    public Button Commands;
    public TableView<TableEntry> Table;
    public TableColumn<TableEntry,String> PlantID;
    public TableColumn<TableEntry,String> Name;
    public TableColumn<TableEntry,String> Current_Temperature;
    public TableColumn<TableEntry,String> Current_Humidity;
    public TableColumn<TableEntry,String> Current_Light;
    public TableColumn<TableEntry,String> Optimal_Temperature;
    public TableColumn<TableEntry,String> Optimal_Humidity;
    public TableColumn<TableEntry,String> Optimal_Light;
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
    void test()
    {
        int i=0;
        while(true)
        {data.get(0).ID.setText((Integer.toString(i)));
        i++;
        }
    }

}
