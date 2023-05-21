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
        stage.setOnCloseRequest(e -> {
            System.exit(0);
        });
        brokerOnline = false;
        connectMqtt();
        initPublish();
        subscribe();
        setCommandPub();
    }

    public void connectMqtt() {
        try {
            mqttController = new MqttController();
            brokerOnline = true;
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                if (mqttController != null) {
                    new WioPubDisconnector(mqttController, mqttController.getClient());
                }
            }));
            timerThread = new Thread(mqttController);
            timerThread.start();
        } catch (Exception e) {
            brokerNotFound();
        }
    }

    private void initPublish() {
        try {
            if (brokerOnline) {
                assert mqttController != null;
                switch (preferences.getInterval()) {
                    case 5:
                        mqttController.publish("pub5;", true);
                        break;
                    case 60:
                        mqttController.publish("pub60;", true);
                        break;
                    case 300:
                        mqttController.publish("pub300;", true);
                        break;
                    case 1800:
                        mqttController.publish("pub1800;", true);
                        break;
                }
            }
        } catch (Exception e) {
            brokerLostConnection();
        }
    }

    private void subscribe() {
        try {
            if (brokerOnline) {
                mqttController.subscribe(this);
            }
        } catch (Exception e) {
            brokerLostConnection();
        }
    }

    public void publish(String input) {
        try {
            if (brokerOnline) {
                assert mqttController != null;
                mqttController.publish(input);
            }
        } catch (Exception e) {
            brokerLostConnection();
        }
    }

    private void brokerLostConnection() {
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

    private void brokerNotFound() {
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

    public void reconnectBroker() {
        brokerOnline = false;
        Platform.runLater(() -> {

            connectMqtt();
            initPublish();
            subscribe();
            setCommandPub();

        });
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
            if (!brokerOnline) {
                connectMqtt();
            } else {
                initPublish();
                subscribe();
            }
        }
    }

    private void setCommandPub() {
        if (brokerOnline) {
            CommandsController.setPublisher(this);
        }
    }

    static void setRoot(String fxml) throws IOException {
        scene.setRoot(loadFXML(fxml));
    }

    private static Parent loadFXML(String fxml) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(App.class.getResource(fxml + ".fxml"));
        return fxmlLoader.load();
    }

    public static void main(String[] args) {
        launch();
    }
}