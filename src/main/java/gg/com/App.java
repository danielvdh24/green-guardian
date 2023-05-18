package gg.com;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;
import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.util.Duration;
import java.io.FileWriter;

import java.io.IOException;
import java.util.Objects;

/**
 * JavaFX App
 */
public class App extends Application {
    private static Scene scene;
    private MqttController mqttController;
    private Thread timerThread;
    public boolean brokerOnline;
    private Timeline countdownTimeline;
    private int remainingSeconds;
    private Preferences preferences;
    private Alert alert;

    @Override
    public void start(Stage stage) throws IOException {
        preferences = Preferences.getPreferences();
        SettingsController.loadDelay();
        scene = new Scene(loadFXML(preferences.getScene()), 600, 388);
        stage.getIcons().add(new Image(Objects.requireNonNull(App.class.getResource("/images/Logo.png")).toString()));
        stage.setTitle("Green Guardian");
        stage.setResizable(false);
        stage.setScene(scene);
        stage.show();
        brokerOnline = false;
        connectMqtt();
    }
    public void connectMqtt() {
        try {
            mqttController = new MqttController();
            brokerOnline = true;
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                if(mqttController != null) {
                    new WioPubDisconnector(mqttController, mqttController.getClient());
                }
            }));
            timerThread = new Thread(mqttController);
            timerThread.start();
        } catch (Exception e) {
            alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Broker not Connected!");
            alert.setHeaderText("MQTT Connection failed or broker disconnected" + "\n"
                    + "Please launch your MQTT broker!");
            countdownTimeline = new Timeline(new KeyFrame(Duration.seconds(1), event -> updateCountdown()));
            countdownTimeline.setCycleCount(Animation.INDEFINITE);
            Platform.runLater(() -> {
                alert.showAndWait();
            });
            startCountdown();
        }
        subscribeAndPublish();
    }

    private void subscribeAndPublish() {
        try {
            if (brokerOnline) {
                assert mqttController != null;
                switch (preferences.getInterval()) {
                    case 5:
                        mqttController.publish("pub5;");
                        break;
                    case 60:
                        mqttController.publish("pub60;");
                        break;
                    case 300:
                        mqttController.publish("pub300;");
                        break;
                    case 1800:
                        mqttController.publish("pub1800;");
                        break;
                }
                mqttController.subscribe(this);
                mqttController.publish("timeScedOn;1200;1500;");

            }
        } catch (Exception e){
            alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Connection Failed!");
            alert.setHeaderText("MQTT Connection failed" + "\n"
                    + "Please restart your MQTT Broker!");
            countdownTimeline = new Timeline(new KeyFrame(Duration.seconds(1), event -> updateCountdown()));
            countdownTimeline.setCycleCount(Animation.INDEFINITE);
            Platform.runLater(() -> {
                alert.showAndWait();
            });
            startCountdown();
        }
    }

    public void reconnectBroker() {
        brokerOnline = false;
        Platform.runLater(this::connectMqtt);
    }

    private void startCountdown() {
        remainingSeconds = 60;
        countdownTimeline.play();
    }

    private void updateCountdown() {
        remainingSeconds--;
        if (remainingSeconds >= 0) {
            String timeText = String.format("Retrying in 0:%02d seconds", remainingSeconds);
            alert.setContentText(timeText);
        } else {
            alert.close();
            countdownTimeline.stop();
            alert = null;
            if(!brokerOnline){
                connectMqtt();
            } else {
                subscribeAndPublish();
            }
        }
    }

    static void setRoot(String fxml) throws IOException {
        scene.setRoot(loadFXML(fxml));
    }

    private static Parent loadFXML(String fxml) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(App.class.getResource(fxml + ".fxml"));
        return fxmlLoader.load();
    }

    public static void main(String[] args)
    {
        /*String result = "";
        byte[] byte1 = new byte[10];

        for(int i=0;i<5;i++)
        {
            byte1[i*2] = (byte)(65 + i);
            byte1[i*2+1] = (byte)(32);
        }

        for(int i=0;i<10;i++)
        {
            char c = (char)byte1[i];
            result += c;
        }

        System.out.println(result);

        try
        {
            FileWriter fileWriter;
            fileWriter = new FileWriter(Objects.requireNonNull(App.class.getClassLoader().getResource("xd.txt")).getFile());
            fileWriter.write(result);
            fileWriter.close();
        }
        catch(Exception e)
        {
            System.out.println("File not found.");
        }*/



        launch();
    }

}
