package gg.com;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.Objects;



/**
 * JavaFX App
 */
public class App extends Application {
    private static Scene scene;
    @Override
    public void start(Stage stage) throws IOException
    {
        Preferences preferences = Preferences.getPreferences();

        SettingsController.loadDelay();
        scene = new Scene(loadFXML(preferences.getScene()), 600, 388);
        stage.getIcons().add(new Image(Objects.requireNonNull(App.class.getResource("/images/Logo.png")).toString()));
        stage.setTitle("Green Guardian");
        stage.setResizable(false);
        stage.setScene(scene);
        stage.show();

        MqttController mqttController = new MqttController();

        switch(preferences.getInterval()){
            case 5 :
                mqttController.publish("pub5;");
                break;
            case 60 :
                mqttController.publish("pub60;");
                break;
            case 300 :
                mqttController.publish("pub300;");
                break;
            case 1800 :
                mqttController.publish("pub1800;");
                break;
        }

        mqttController.subscribe();

        Runtime.getRuntime().addShutdownHook(new WioPubDisconnector(mqttController, mqttController.getClient()));
    }
    static void setRoot(String fxml) throws IOException
    {
        scene.setRoot(loadFXML(fxml));
    }

    private static Parent loadFXML(String fxml) throws IOException
    {
        FXMLLoader fxmlLoader = new FXMLLoader(App.class.getResource(fxml + ".fxml"));
        return fxmlLoader.load();
    }

    public static void main(String[] args)
    {
        launch();
    }

}