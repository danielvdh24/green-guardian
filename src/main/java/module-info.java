module gg.com {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.google.gson;

    opens gg.com to javafx.fxml, com.google.gson;
    exports gg.com;
}
