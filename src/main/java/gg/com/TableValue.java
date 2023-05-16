package gg.com;

import java.util.ArrayList;

import javafx.beans.InvalidationListener;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;

public class TableValue implements ObservableValue<String>{
    
        String text;
        ArrayList<InvalidationListener> listeners;

        TableValue(String text)
        {
            this.listeners = new ArrayList<>();
            this.text = text;
        }

        public void addListener(InvalidationListener arg0) 
        {
            listeners.add(arg0);
        }

        public void removeListener(InvalidationListener arg0) 
        {
            listeners.remove(arg0);
        }

        public void addListener(ChangeListener<? super String> arg0) 
        {
            throw new UnsupportedOperationException("Unimplemented method 'addListener'");
        }

        public void removeListener(ChangeListener<? super String> arg0) 
        {
            throw new UnsupportedOperationException("Unimplemented method 'removeListener'");
        }

        public void setText(String text)
        {
            this.text = text;
            for (InvalidationListener invalidationListener : listeners) {
                invalidationListener.notify();
            }
        }

        public String getValue()
        {
            return text;
        }

}
