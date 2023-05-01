package gg.com;

import java.util.HashSet;
import javafx.beans.InvalidationListener;
import javafx.beans.value.ObservableValue;

public class TableEntry {
    
    HashSet<InvalidationListener> listeners;
    public TableValue ID;
    public TableValue name;
    public TableValue CT;
    public TableValue CH;
    public TableValue CL;
    public TableValue OT;
    public TableValue OH;
    public TableValue OL;
    public TableValue status;

    TableEntry(Integer ID, String name,int CT,int CH,int CL,int OT,int OH,int OL,String status)
    {
        this.name = new TableValue(name);
        this.ID = new TableValue(Integer.toString(OL));
        this.CH = new TableValue(Integer.toString(CH));
        this.CT = new TableValue(Integer.toString(CT));
        this.CL = new TableValue(Integer.toString(CL));
        this.OH = new TableValue(Integer.toString(OH));
        this.OT = new TableValue(Integer.toString(OT));
        this.OL = new TableValue(Integer.toString(OL));
        this.status = new TableValue(status);
    }

    public ObservableValue<String> IDProperty() {
        return ID;
    }

    public ObservableValue<String> nameProperty() {
        return name;
    }

    public ObservableValue<String> CHProperty() {
        return CH;
    }

    public ObservableValue<String> CTProperty() {
        return CT;
    }

    public ObservableValue<String> CLProperty() {
        return CL;
    }

    public ObservableValue<String> OHProperty() {
        return OH;
    }

    public ObservableValue<String> OTProperty() {
        return OT;
    }

    public ObservableValue<String> OLProperty() {
        return OL;
    }

    public ObservableValue<String> statusProperty() {
        return status;
    }

}
