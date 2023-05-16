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
import java.util.Objects;

public class GraphController {
    private final String[] sources = {"Moisture","Temperature","Light"};
    @FXML
    private LineChart<String, Number> chart;
    @FXML
    private NumberAxis yAxis;
    @FXML   
    private ChoiceBox<String> dataSelect;
    @FXML
    public void initialize(){
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
        Reader reader = new Reader(Objects.requireNonNull(App.class.getClassLoader().getResource("test.txt")).getFile());
        if(dataSelect.getValue().equals("Light"))
        {
            reader.nextInt();
            reader.nextInt();
        }
        if(dataSelect.getValue().equals("Temperature"))
        {
            reader.nextInt();
        }
        int count = 1;
        chart.getData().add(series);
        addDataWithDelay(series, reader, count);
    }

    private void addDataWithDelay(XYChart.Series<String, Number> series, Reader reader, int count) {
        if (count > 336) { // temporary limit
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

    private void SwitchData(ActionEvent event)
    {
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