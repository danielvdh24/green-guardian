package gg.com;

import javafx.scene.control.ChoiceBox;
import javafx.animation.PauseTransition;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.util.Duration;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class GraphController {
    private final String[] sources = {"Moisture", "Temperature", "Light"};
    @FXML
    private LineChart<String, Number> chart;
    @FXML
    private NumberAxis yAxis;
    @FXML
    private ChoiceBox<String> dataSelect;

    @FXML
    public void initialize() {
        Path dataFilePath = Paths.get("config", "GraphData.txt");
        try {
            if (!Files.exists(dataFilePath)) {
                Files.createFile(dataFilePath);
                String Line = "0" + " " + "0" + " " + "0" + "\n";
                String NewData = "";
                System.out.println(Line);
                for (int i = 0; i < 336; i++) NewData += Line;
                DocWriter.write("GraphData.txt", NewData);
            }
            
        } catch (Exception e) {
            System.out.println("Folder not found.");
        }

        dataSelect.getItems().addAll(sources);
        dataSelect.setOnAction(this::SwitchData);
        dataSelect.setValue(sources[0]);
        yAxis.setLowerBound(0);
        yAxis.setUpperBound(100);
        launchGraph();
    }

    @FXML
    protected void launchGraph() {
        chart.getData().clear();
        XYChart.Series<String, Number> series = new XYChart.Series<>();
        Reader reader = new Reader("GraphData.txt");
        if (dataSelect.getValue().equals("Moisture")) {
            reader.nextInt();
            reader.nextInt();
        }
        if (dataSelect.getValue().equals("Temperature")) {
            reader.nextInt();
        }
        int count = 1;
        chart.getData().add(series);
        addDataWithDelay(series, reader, count);
    }

    private void addDataWithDelay(XYChart.Series<String, Number> series, Reader reader, int count) {
        if (count > 335) { // temporary limit
            return;
        }
        String count1 = Integer.toString(count);
        PauseTransition pause = new PauseTransition(Duration.seconds(0));
        pause.setOnFinished(event -> {
            series.getData().add(new XYChart.Data<>(count1, reader.nextInt()));
            reader.nextInt();
            reader.nextInt();
            addDataWithDelay(series, reader, count + 1);
        });
        pause.play();
    }

    private void SwitchData(ActionEvent event) {
        launchGraph();
    }

    public void onSpreadSheetButtonClick() throws IOException {
        App.setRoot("SpreadsheetScene");
    }

    public void onCommandsButtonClick() throws IOException {
        App.setRoot("CommandsScene");
    }

    public void onSettingsButtonClick() throws IOException {
        App.setRoot("SettingsScene");
    }
}