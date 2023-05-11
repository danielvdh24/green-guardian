package gg.com;

import javafx.animation.PauseTransition;
import javafx.fxml.FXML;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.util.Duration;

import java.io.IOException;
import java.util.Objects;

public class GraphController {
    @FXML
    private LineChart<String, Number> chart;
    @FXML
    private NumberAxis yAxis;

    @FXML
    public void initialize(){
        yAxis.setLowerBound(0);
        yAxis.setUpperBound(300);
        launchGraph();
    }

    @FXML
    protected void launchGraph() {
        XYChart.Series<String, Number> series = new XYChart.Series<>();
        Reader reader = new Reader(Objects.requireNonNull(App.class.getClassLoader().getResource("test.txt")).getFile());
        int count = 1;
        chart.getData().add(series);
        addDataWithDelay(series, reader, count);
    }

    private void addDataWithDelay(XYChart.Series<String, Number> series, Reader reader, int count) {
        if (count > 100) { // temporary limit
            return;
        }
        String count1 = Integer.toString(count);
        PauseTransition pause = new PauseTransition(Duration.seconds(SettingsController.getDelay()));
        pause.setOnFinished(event -> {
            series.getData().add(new XYChart.Data<>(count1, reader.nextInt()));
            addDataWithDelay(series, reader, count + 1);
        });
        pause.play();
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