package gg.com;

import com.google.gson.Gson;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Preferences {
    
    public static final String CONFIG_FILE = "config.txt";
    public static final String CONFIG_DIRECTORY = "config";

    String scene;
    int interval;
    boolean notifications;
    boolean online;
    boolean manual;
    boolean light;
    String startTime;
    String endTime;

    private Preferences() {
        //default values if no config file found
        scene = "SpreadsheetScene";
        interval = 5;
        notifications = false;
        online = true;
        manual = false;
        light = true;
        startTime = "1800";
        endTime = "0500";
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

    public boolean getOnline() {
        return online;
    }

    public void setOnline(boolean online) {
        this.online = online;
    }

    public boolean getManual() {
        return manual;
    }

    public void setManual(boolean manual) {
        this.manual = manual;
    }

    public boolean getLight() {
        return light;
    }

    public void setLight(boolean light) {
        this.light = light;
    }

    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }

    public String getStartTime() {
        return startTime;
    }

    public void setEndTime(String endTime) {
        this.endTime = endTime;
    }

    public String getEndTime() {
        return endTime;
    }

    public static void initConfig() {
        Writer writer = null;
        try {
            Preferences preference = new Preferences();
            Gson gson = new Gson();
            Path configDirectoryPath = Paths.get(CONFIG_DIRECTORY);
            if (!Files.exists(configDirectoryPath)) {
                Files.createDirectories(configDirectoryPath);
            }
            Path configFilePath = Paths.get(CONFIG_DIRECTORY, CONFIG_FILE);
            if (!Files.exists(configFilePath)) {
                Files.createFile(configFilePath);
                writer = Files.newBufferedWriter(configFilePath);
                gson.toJson(preference, writer);
            }
        } catch (IOException e) {
        } finally {
            try {
                if (writer != null) {
                    writer.close();
                }
            } catch (IOException e) {}
        }
    }

    public static Preferences getPreferences() {
        Gson gson = new Gson();
        Preferences preferences = new Preferences();
        try {
            Path configFilePath = Paths.get(CONFIG_DIRECTORY, CONFIG_FILE);
            if (Files.exists(configFilePath)) {
                BufferedReader reader = Files.newBufferedReader(configFilePath);
                preferences = gson.fromJson(reader, Preferences.class);
            } else {
                initConfig();
            }
        } catch (Exception e) {}
        return preferences;
    }

    public static void writePreferenceToFile(Preferences preference) {
        try {
            Gson gson = new Gson();
            Path configDirectoryPath = Paths.get(CONFIG_DIRECTORY);
            if (!Files.exists(configDirectoryPath)) {
                Files.createDirectories(configDirectoryPath);
            }
            Path configFilePath = Paths.get(CONFIG_DIRECTORY, CONFIG_FILE);
            Writer writer = Files.newBufferedWriter(configFilePath);
            gson.toJson(preference, writer);
            writer.close();
        } catch (IOException e) {}
    }
}