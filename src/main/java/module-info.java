module gg.com {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.google.gson;
    requires org.eclipse.paho.client.mqttv3;

    opens gg.com to javafx.fxml, com.google.gson;
    exports gg.com;
}
