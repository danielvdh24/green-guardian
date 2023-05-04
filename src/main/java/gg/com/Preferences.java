package gg.com;
import com.google.gson.Gson;

import java.io.*;

public class Preferences {
    public static final String CONFIG_FILE = "src/main/resources/config.txt";

    String scene;
    int interval;
    boolean notifications;

    public Preferences(){ // default values if no config file found
        scene = "SpreadSheetScene";
        interval = 5;
        notifications = false;
    }

    public String getScene() {
        return scene;
    }

    public void setScene(String scene) {
        this.scene = scene;
    }

    public int getInterval() {
        return interval;
    }

    public void setInterval(int interval) {
        this.interval = interval;
    }

    public boolean isNotifications() {
        return notifications;
    }

    public void setNotifications(boolean notifications) {
        this.notifications = notifications;
    }

    public static void initConfig(){
        Writer writer = null;
        try {
            Preferences preference = new Preferences();
            Gson gson = new Gson();
            writer = new FileWriter(CONFIG_FILE);
            gson.toJson(preference, writer);
        } catch (IOException e){
            e.printStackTrace();
        } finally {
            try {
                assert writer != null;
                writer.close();
            } catch (IOException e){
                e.printStackTrace();
            }
        }
    }

    public static Preferences getPreferences(){
        Gson gson = new Gson();
        Preferences preferences = new Preferences();
        try {
            preferences = gson.fromJson(new FileReader(CONFIG_FILE), Preferences.class);
        } catch (FileNotFoundException e){
            initConfig();
        }
        return preferences;
    }

    public static void writePreferenceToFile(Preferences preference){
        Writer writer = null;
        try {
            Gson gson = new Gson();
            writer = new FileWriter(CONFIG_FILE);
            gson.toJson(preference, writer);
        } catch (IOException e){
            e.printStackTrace();
        } finally {
            try {
                assert writer != null;
                writer.close();
            } catch (IOException e){
                e.printStackTrace();
            }
        }
    }

}
