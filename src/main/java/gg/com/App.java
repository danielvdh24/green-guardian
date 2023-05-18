package gg.com;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

import java.io.FileWriter;
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