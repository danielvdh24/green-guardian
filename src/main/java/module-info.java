module gg.com {
    requires javafx.controls;
    requires javafx.fxml;

    opens gg.com to javafx.fxml;
    exports gg.com;
}
